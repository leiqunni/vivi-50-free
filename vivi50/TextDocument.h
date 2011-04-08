//----------------------------------------------------------------------
//
//			File:			"TextDocument.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextDocument �N���X�錾
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	�{�\�[�X�R�[�h�͊�{�I�� MIT ���C�Z���X�ɏ]���B
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	�������A������͕s���R�Ŏg������̈��� GPL ���匙���Ȃ̂ŁA
	GPL ���C�Z���X�v���W�F�N�g���{�\�[�X�𗬗p���邱�Ƃ��ւ���

*/

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <QObject>
#include	<vector>
#include	<boost/pool/object_pool.hpp>
#include	"gap_vector.h"
//#include	"buffer.h"

typedef unsigned char uchar;
typedef const unsigned char cuchar;

//typedef std::gap_vector<uchar> GV_utf8;

//#define		BLOCK_HAS_OFFSET	0
#define		BLOCK_HAS_SIZE		1

class TextDocument;
class TextBlock;

//----------------------------------------------------------------------
struct TextBlockData
{
public:
	index_t		m_index;
	index_t		m_position;
public:
	TextBlockData(index_t index = 0, index_t position = 0)
		: m_index(index), m_position(position)
		{}

public:
	index_t index() const { return m_index; }
	index_t position() const { return m_position; }
};
//----------------------------------------------------------------------
//	undo/redo ������̊i�[�Ƀq�[�v��p���AundoItem �N���X�͌p������߁Atype �Ńf�B�X�p�b�`����
//#define		UNDOMGR_USE_HEAP			1

enum {
	BBUNDOITEM_UNDO_MF_OFF = 0x0001,	//	Undo �Ń��f�B�t�@�C�t���O���n�m����n�e�e��
	BBUNDOITEM_REDO_MF_OFF = 0x0002,	//	Redo �Ń��f�B�t�@�C�t���O���n�m����n�e�e��
	BBUNDOITEM_MADELINE = 0x0004,
	BBUNDOITEM_CUR_TAIL = 0x0008,		//	�폜�̎��A�J�[�\�����Ō�ɂ�����
	BBUNDOITEM_SHIFT_LEFT = 0x0010,
	BBUNDOITEM_SHIFT_RIGHT = 0x0000,
	BBUNDOITEM_CONCAT_LINE = 0x0020,	//	�}��������A���\
	BBUNDOITEM_CONCAT_ALL = 0x0040,		//	�}��������A���\
	BBUNDOITEM_DELETE = 0x0080,			//	or BackSpace
	BBUNDOITEM_MARK_POS = 0x0100,		//	CUndoItemMarkPos ���ǂ���
	BBUNDOITEM_BLOCK = 0x0200,			//	CUndoItemBlock ���ǂ���
	BBUNDOITEM_SAVED = 0x0400,			//	�h�L�������g���ۑ����ꂽ
	BBUNDOITEM_DIFF_RESTORE = 0x0800,		//	diff �ő���t�@�C���ɂ��킹��
	BBUNDOITEM_REPTEXT = 0x1000,		//	������̒u�����s����
	BBUNDOITEM_TRANSLATE = 0x2000,		//	�R�[�h�ϊ��̏ꍇ
	BBUNDOITEM_REDRAW = 0x4000,			//	:m �̏ꍇ�̗l�ɋ����I�Ƀ��h���[����
	BBUNDOITEM_INSTEXT = 0x8000,		//	������̑}�����s����
};

enum {
	BBUNDOITEM_TYPE_RESIZE = 1,
	BBUNDOITEM_TYPE_INSERT,
	BBUNDOITEM_TYPE_ERASE,
	BBUNDOITEM_TYPE_REPLACE,
};


struct GVUndoItem
{
public:
	const uchar	m_type;
	bool	m_beforeSave:1;		//	�ۑ��O�� undo/redo item
	short	m_flags;
	const index_t	m_first;
	index_t	m_last;				//	�}���E�폜�E�u���O�͈�
	index_t	m_last2;			//	�u����͈̔�
	index_t	m_hp_ix;
	index_t	m_rhp_ix;			//	redo �p�q�[�v�C���f�b�N�X
	//size_t	m_hpSize;			//	�q�[�v�Ɋi�[����Ă���f�[�^�T�C�Y�i�o�C�g���j
//protected:
public:
	GVUndoItem()
		: m_type(0)
		, m_first(0), m_last(0), m_last2(0)
		, m_hp_ix(0), m_rhp_ix(0)
		, m_flags(0)
		, m_beforeSave(false)
	{}
	GVUndoItem(uchar type,
				index_t first, index_t last, index_t hp_ix, index_t last2 = 0,
				short flags = 0)
		: m_type(type)
		, m_first(first), m_last(last), m_last2(last2)
		, m_hp_ix(hp_ix), m_rhp_ix(0)
		, m_flags(flags)
		, m_beforeSave(false)
	{}
public:
	size_t	data_size() const { return m_last - m_first; }
	size_t	data_size2() const { return m_last2 - m_first; }
};
class GVUndoMgr
{
	uint	m_current;		//	�I�u�W�F�N�g�����ɓ����ʒu�i0..*�j
							//	�ʏ�� m_item.size() �Ɠ��l�����Aundo �����s�����ƃf�N�������g�����
	std::vector<GVUndoItem*>	m_items;
	std::vector<uchar>	m_heap;			//	undo �̂��߂̕�������i�[����q�[�v
	std::vector<uchar>	m_redoHeap;		//	redo �̂��߂̕�������i�[����q�[�v
public:
	GVUndoMgr() : m_current(0)
	{
		m_heap.push_back(1);			//	�ŏ��̃f�[�^�C���f�b�N�X�� 1 �ɂ��邽�߂̃_�~�[
		m_redoHeap.push_back(1);
	}
	~GVUndoMgr()
	{
		//std::cout << "m_items.size() = " << m_items.size() << "\n";
	}
public:
	bool	canUndo() const { return m_current != 0; };
	bool	canRedo() const { return m_current < m_items.size(); };
public:
	void	push_back(GVUndoItem *ptr, bool = false);

	//	�f�[�^���q�[�v�ɒǉ�
	template<typename InputIterator>
	index_t	addToHeap(InputIterator first, InputIterator last)
	{
		const index_t sz = m_heap.size();
		m_heap.insert(m_heap.end(), first, last);
		return sz;
	}
#if 0
	index_t	addToHeap(uchar v)
	{
		const index_t sz = m_heap.size();
		m_heap.push_back(v);
		return sz;
	}
#endif
	template<typename InputIterator>
	index_t	addToRedoHeap(InputIterator first, InputIterator last)
	{
		const index_t sz = m_redoHeap.size();
		m_redoHeap.insert(m_redoHeap.end(), first, last);
		return sz;
	}
#if 0
	index_t	addToRedoHeap(uchar v)
	{
		const index_t sz = m_redoHeap.size();
		m_redoHeap.push_back(v);
		return sz;
	}
#endif
	bool	doUndo(TextDocument*, uint&);
	bool	doRedo(TextDocument*, uint&);
};
//----------------------------------------------------------------------
#define		INVALID_INDEX		0xffffffff

class TextCursor
{
public:
	enum {
		MoveAnchor = 0,
		KeepAnchor,
	};
	enum {
		NoMove = 0,
		Right,
		Left,
		Up,
		Down,
		StartOfBlock,
		EndOfBlock,
		StartOfDocument,
		EndOfDocument,
	};
public:
#if	BLOCK_HAS_SIZE
	TextCursor(TextDocument *document = 0, index_t position = 0)
		: m_document(document), m_position(position), m_anchor(position)
		{ updateBlockData(); }
	TextCursor(TextDocument *document, index_t position, index_t anchor)
		: m_document(document), m_position(position), m_anchor(anchor)
		//, m_blockIndex(0), m_blockPosition(0)
		{ updateBlockData(); }
	TextCursor(TextDocument *document, index_t position, index_t anchor,
				TextBlockData blockData)
		: m_document(document), m_position(position), m_anchor(anchor)
		, m_blockData(blockData)
		{}
#if 0
	TextCursor(TextDocument *document, index_t position, index_t anchor,
				index_t blockIx, index_t blockPosition)
		: m_document(document), m_position(position), m_anchor(anchor)
		, m_blockIndex(blockIx), m_blockPosition(blockPosition)
		{}
#endif
#else
	TextCursor(TextDocument *document = 0, index_t position = 0)
		: m_document(document), m_position(position), m_anchor(position)
		{}
	TextCursor(TextDocument *document, index_t position, index_t anchor)
		: m_document(document), m_position(position), m_anchor(anchor)
		{}
#endif
	TextCursor(const TextCursor &x)
		: m_document(x.m_document), m_position(x.m_position), m_anchor(x.m_anchor)
#if	BLOCK_HAS_SIZE
		, m_blockData(x.m_blockData), m_anchorBlockData(x.m_anchorBlockData)
		//, m_blockIndex(x.m_blockIndex), m_blockPosition(x.m_blockPosition)
#endif
		{}
	~TextCursor() {}

public:
	TextDocument	*document() const { return m_document; }
	index_t	position() const { return m_position; }
	index_t	anchor() const { return m_anchor; }
	int		prevCharsCount() const;		//	�s������J�[�\���܂ł̕�������Ԃ�
	bool	hasSelection() const { return m_position != m_anchor; }
	bool	isNull() const { return m_document == 0; }
	bool	atEnd() const;	// { return isNull() || m_position >= m_document->size(); }
	QString	selectedText() const;
#if	BLOCK_HAS_SIZE
	TextBlockData blockData() const { return m_blockData; }
	TextBlockData anchorBlock() const { return m_anchorBlockData; }
	index_t	blockIndex() const { return m_blockData.m_index; }
	index_t	blockPosition() const { return m_blockData.m_position; }
	index_t	ancBlockIndex() const { return m_anchorBlockData.m_index; }
	index_t	ancBlockPosition() const { return m_anchorBlockData.m_position; }
#endif

public:
	void	clearSelection() { copyPositionToAnchor(); }
	TextBlock	block();
	void	copyPositionToAnchor();
	void	copyAnchorToPosition();
	void	swapPositionAnchor();
	void	setPosition(index_t position, uchar mode = MoveAnchor);
	bool	movePosition(uchar move, uchar mode = MoveAnchor, uint n = 1);

	void	insertText(const QString &);
	void	deleteChar();
	void	deletePreviousChar();

protected:
	void	updateBlockData(uchar mode = MoveAnchor);		//	m_blockIndex, m_blockPosition �X�V

private:
	TextDocument	*m_document;
	index_t			m_position;		//	�J�[�\���ʒu
	index_t			m_anchor;		//	�A���J�[�ʒu
#if	BLOCK_HAS_SIZE
	TextBlockData	m_blockData;
	TextBlockData	m_anchorBlockData;
#if 0
	index_t		m_blockIndex;			//	�u���b�N�C���f�b�N�X
	index_t		m_blockPosition;		//	�u���b�N�擪�ʒu
	index_t		m_ancBlockIndex;		//	�u���b�N�C���f�b�N�X
	index_t		m_ancBlockPosition;		//	�u���b�N�擪�ʒu
#endif
#endif
};

class TextBlock
{
public:
#if	BLOCK_HAS_SIZE
	//TextBlock(TextDocument *document, index_t blockNumber)
	//	: m_document(document), m_blockNumber(blockNumber)
	//	{}
	TextBlock(TextDocument *document, index_t blockNumber, index_t blockPosition)
		: m_document(document), m_data(TextBlockData(blockNumber, blockPosition))
		{}
	TextBlock(TextDocument *document, TextBlockData block)
		: m_document(document), m_data(block)
		{}
#endif
	TextBlock(const TextBlock &x)
		: m_document(x.m_document)
#if	BLOCK_HAS_SIZE
		, m_data(x.m_data)
#endif
		{}
	~TextBlock() {}

public:
	uint		size() const;		//	���s���܂߂��R�[�h��
	uint		length() const { return size(); }
	bool		isValid() const { return blockNumber() != INVALID_INDEX; }
	index_t		index() const { return m_data.m_index; }
	index_t		blockNumber() const { return m_data.m_index; }
	index_t		position() const;	// { return isValid() ? m_document->blockPosition(m_index) : 0; }
	QString		text() const;
	int			charsCount(index_t) const;		//	�s������w��ʒu�܂ł̕�������Ԃ�

	bool	operator==(const TextBlock &x) const
	{ return m_document == x.m_document && blockNumber() == x.blockNumber(); }
	bool	operator!=(const TextBlock &x) const
	{ return !this->operator==(x); }

public:
	TextBlock	next() const;

private:
	TextDocument	*m_document;
#if	BLOCK_HAS_SIZE
	TextBlockData	m_data;
	//index_t			m_blockNumber;		//	�u���b�N�z��C���f�b�N�X 0..*
	//index_t			m_blockPosition;
#endif
};

//----------------------------------------------------------------------
#if 1
//	����̓u���b�N�T�C�Y�݂̂����A�ߖ����� �t���O�ނ�ǉ�����
struct TextBlockItem
{
	size_t		m_size;		//	�u���b�N�������T�C�Y
public:
	TextBlockItem(size_t size = 0) : m_size(size) {}
};
#endif
//----------------------------------------------------------------------
class TextDocument : public QObject
{
	Q_OBJECT

public:
	TextDocument(QObject *parent = 0);
	~TextDocument();

public:
	const QString fullPath() const { return m_fullPath; }
	bool	isModified() const { return m_modified; }
	bool	isEmpty() const { return m_buffer.empty(); }
	size_t	size() const { return m_buffer.size(); }
	size_t	blockCount() const { return m_blocks.size(); }
	size_t	blockPosition(index_t ix) const;
	size_t	blockSize(index_t ix) const;
	uchar	operator[](index_t ix) const { return m_buffer[ix]; }
	QString	toPlainText() const;
	bool	isMatch(index_t, cuchar *, cuchar *) const;		//	�P����r�֐�
	uchar	charEncoding() const { return m_charEncoding; }
	bool	withBOM() const { return m_withBOM; }

	TextBlockData	findBlockData(index_t position) const;
	TextBlockData	nextBlockData(TextBlockData d) const
	{ return TextBlockData(d.m_index + 1, d.m_position + m_blocks[d.m_index].m_size); }
	TextBlockData	prevBlockData(TextBlockData d) const
	{ return TextBlockData(d.m_index - 1, d.m_position - m_blocks[d.m_index - 1].m_size); }

public:
	bool	canUndo() const { return m_undoMgr.canUndo(); };
	bool	canRedo() const { return m_undoMgr.canRedo(); };

public:
	void	init();
	void	clear() { init(); }

	void	setFullPath(const QString &fullPath) { m_fullPath = fullPath; }

#if BLOCK_HAS_SIZE
	TextBlock	firstBlock() { return TextBlock(this, 0, 0); }
	TextBlock	lastBlock()
	{
		return TextBlock(this, blockCount() - 1,
						size() - m_blocks[blockCount() - 1].m_size);
	}
#else
	TextBlock	firstBlock() { return TextBlock(this, 0); }
	TextBlock	lastBlock() { return TextBlock(this, blockCount() - 1); }
#endif
	TextBlock	findBlock(index_t);
	index_t		findBlockIndex(index_t position, index_t *pBlockPos = 0) const;
	TextBlock	findBlockByNumber(index_t);		//	�u���b�N�ԍ��i0..*�j����u���b�N���擾
	TextBlock	findBlockByNumberRaw(index_t);		//	�u���b�N�ԍ��i0..*�j����u���b�N���擾

	void	erase(index_t, index_t);
	void	erase(index_t, TextBlockData, index_t);
	void	insert(index_t, const QString &);
	void	insert(index_t ix, cuchar *first, cuchar *last);
	void	insert(index_t, TextBlockData, const QString &);
	void	insert(index_t ix, TextBlockData, cuchar *first, cuchar *last);
	void	setPlainText(const QString &);
	//void	append(const QByteArray &);		//	UTF-8 �z��
	//void	updateBlocksAtInsert(index_t, size_t);
	void	updateBlocksAtInsert(index_t, TextBlockData, size_t);
	//void	updateBlocksAtErase(index_t, index_t);
	void	updateBlocksAtErase(index_t, TextBlockData, index_t);

	void	do_insert(index_t, const QString &);	//	undo/redo �Ή���
	void	do_erase(index_t, index_t);				//	undo/redo �Ή���
	void	do_replace(index_t, index_t, const QString &);		//	undo/redo �Ή���

	void	insertText(TextCursor&, const QString &);
	void	deleteChar(TextCursor&);
	void	deletePreviousChar(TextCursor&);

	void	doUndo(index_t &pos) { m_undoMgr.doUndo(this, pos); }
	void	doRedo(index_t &pos) { m_undoMgr.doRedo(this, pos); }
#if 0
	void	doUndo()
	{
		uint ix;
		m_undoMgr.doUndo(this, ix);
	}
	void	doRedo()
	{
		uint ix;
		m_undoMgr.doRedo(this, ix);
	}
#endif

	TextCursor	find(const QString &, index_t = 0);
	void	setCharEncodeing(uchar ce) { m_charEncoding = ce; }
	void	setWithBOM(bool b) { m_withBOM = b; }

public:
	std::gap_vector<uchar>::iterator	begin() { return m_buffer.begin(); }
	std::gap_vector<uchar>::iterator	end() { return m_buffer.end(); }

public:
	//void	do_unitTest();		//	�P�̃e�X�g

signals:
	void contentsChange( index_t position, size_t charsRemoved, size_t charsAdded );
	void contentsChanged();
	void blockCountChanged();

protected:
	void	buildBlocks();

private:
	index_t	findBlockIndex(index_t first, index_t last, index_t val) const;

private:
	QString	m_fullPath;
	uchar	m_charEncoding;
	bool	m_withBOM;
	bool	m_modified;
	mutable std::gap_vector<uchar>	m_buffer;
	//mutable std::gap_vector<size_t>	m_blocks;		//	�u���b�N�T�C�Y�z��
	mutable std::gap_vector<TextBlockItem>	m_blocks;		//	�u���b�N�z��
	TextBlockData	m_block;			//	�J�����g�u���b�N���
	//index_t		m_blockIndex;		//	�J�����g�u���b�N���
	//index_t		m_blockPosition;	//	�J�����g�u���b�N���
	//CBuffer_GV	m_buffer;		//	����UTF-8�ȃo�b�t�@
	GVUndoMgr	m_undoMgr;
	boost::object_pool<GVUndoItem>	m_pool_undoItem;

	friend void test_TextDocument();
};

#endif // TEXTDOCUMENT_H
