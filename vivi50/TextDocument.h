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
//#include	<boost/pool/object_pool.hpp>
#include	"gap_vector.h"
//#include	"buffer.h"
#include	"textBlockData.h"

typedef unsigned char uchar;
typedef const unsigned char cuchar;

#define		INVALID_INDEX		0xffffffff
class TextCursor;

//typedef std::gap_vector<uchar> GV_utf8;

//#define		BLOCK_HAS_OFFSET	0
#define		BLOCK_HAS_SIZE		1
enum {
	CT_EOF = 0,
	CT_NEWLINE,
	CT_SPACE,
	CT_ALNUM,		//	���p�p����
	CT_HANKANA,		//	���p�J�i
	CT_SYM,			//	���p�V���{��
	//CT_DBSPACE,		//	�S�p��
	CT_HIRA,		//	�S�p�Ђ炪��
	CT_KANA,		//	�S�p�J�^�J�i
	CT_DBALNUM,		//	�S�p�p����
	CT_KANJI,		//	����
	//CT_SYM,			//	�S�p�L��
	CT_CONT,		//	�J��Ԃ��L���i�X�Ȃǁj
	CT_GREEK,		//	�M���V������
	CT_ALPHA_EX,	//	���̑��̍��̕���
	CT_OTHER,		//	���̑�����
};

class TextDocument;
class TextBlock;

//----------------------------------------------------------------------
//	undo/redo ������̊i�[�Ƀq�[�v��p���AundoItem �N���X�͌p������߁Atype �Ńf�B�X�p�b�`����
//#define		UNDOMGR_USE_HEAP			1

enum {
	GVUNDOITEM_UNDO_MF_OFF = 0x0001,	//	Undo �Ń��f�B�t�@�C�t���O���n�m����n�e�e��
	GVUNDOITEM_REDO_MF_OFF = 0x0002,	//	Redo �Ń��f�B�t�@�C�t���O���n�m����n�e�e��
	GVUNDOITEM_MADELINE = 0x0004,
	GVUNDOITEM_CUR_TAIL = 0x0008,		//	�폜�̎��A�J�[�\�����Ō�ɂ�����
	GVUNDOITEM_SHIFT_LEFT = 0x0010,
	GVUNDOITEM_SHIFT_RIGHT = 0x0000,
	GVUNDOITEM_CONCAT_LINE = 0x0020,	//	�}��������A���\
	GVUNDOITEM_CONCAT_ALL = 0x0040,		//	�}��������A���\
	GVUNDOITEM_DELETE = 0x0080,			//	or BackSpace
	GVUNDOITEM_MARK_POS = 0x0100,		//	CUndoItemMarkPos ���ǂ���
	GVUNDOITEM_BLOCK = 0x0200,			//	CUndoItemBlock ���ǂ���
	GVUNDOITEM_SAVED = 0x0400,			//	�h�L�������g���ۑ����ꂽ
	GVUNDOITEM_DIFF_RESTORE = 0x0800,		//	diff �ő���t�@�C���ɂ��킹��
	GVUNDOITEM_REPTEXT = 0x1000,		//	������̒u�����s����
	GVUNDOITEM_TRANSLATE = 0x2000,		//	�R�[�h�ϊ��̏ꍇ
	GVUNDOITEM_REDRAW = 0x4000,			//	:m �̏ꍇ�̗l�ɋ����I�Ƀ��h���[����
	GVUNDOITEM_INSTEXT = 0x8000,		//	������̑}�����s����
};

enum {
	GVUNDOITEM_TYPE_RESIZE = 1,
	GVUNDOITEM_TYPE_INSERT,
	GVUNDOITEM_TYPE_ERASE,
	GVUNDOITEM_TYPE_REPLACE,
};


struct GVUndoItem
{
public:
	uchar	m_type;
	bool	m_beforeSave:1;		//	�ۑ��O�� undo/redo item
	short	m_flags;
	index_t	m_first;
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

#if 1
public:
	GVUndoItem &operator=(const GVUndoItem &x)
	{
		m_type = x.m_type;
		m_beforeSave = x.m_beforeSave;
		m_flags = x.m_flags;
		m_first = x.m_first;
		m_last = x.m_last;
		m_last2 = x.m_last2;
		m_hp_ix = x.m_hp_ix;
		m_rhp_ix = x.m_rhp_ix;
		return *this;
	}
#endif
};
class GVUndoMgr
{
	uint	m_current;		//	�I�u�W�F�N�g�����ɓ����ʒu�i0..*�j
							//	�ʏ�� m_item.size() �Ɠ��l�����Aundo �����s�����ƃf�N�������g�����
	std::vector<GVUndoItem>	m_items;
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
	void	push_back(const GVUndoItem &, bool = false);

	//	�f�[�^���q�[�v�ɒǉ�
	template<typename InputIterator>
	index_t	addToHeap(InputIterator first, InputIterator last)
	{
		const index_t sz = m_heap.size();
		m_heap.insert(m_heap.end(), first, last);
		return sz;
	}
	template<typename InputIterator>
	index_t	addToRedoHeap(InputIterator first, InputIterator last)
	{
		const index_t sz = m_redoHeap.size();
		m_redoHeap.insert(m_redoHeap.end(), first, last);
		return sz;
	}
	void	resetModifiedFlags();	//	�h�L�������g���ۑ����ꂽ���ɃR�[�������
	bool	doUndo(TextDocument*, uint&, uint&);
	bool	doRedo(TextDocument*, uint&, uint&);
};
//----------------------------------------------------------------------

class TextBlock
{
public:
	//TextBlock(TextDocument *document, index_t blockNumber)
	//	: m_document(document), m_blockNumber(blockNumber)
	//	{}
	TextBlock(TextDocument *document, index_t blockNumber, index_t blockPosition)
		: m_document(document), m_data(TextBlockData(blockNumber, blockPosition))
		{}
	TextBlock(TextDocument *document, TextBlockData block)
		: m_document(document), m_data(block)
		{}
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
	uint		EOLOffset() const;
	uint		newlineLength() const;		//	���s�����̃o�C�g����Ԃ�
	bool		isValid() const { return m_document == 0 || blockNumber() != INVALID_INDEX; }
	index_t		index() const { return m_data.m_index; }
	index_t		blockNumber() const { return m_data.m_index; }
	index_t		position() const;	// { return isValid() ? m_document->blockPosition(m_index) : 0; }
	TextBlockData	data() const { return m_data; }
	QString		text() const;
	int			charsCount(index_t) const;		//	�s������w��ʒu�܂ł̕�������Ԃ�

	bool	operator==(const TextBlock &x) const
	{ return m_document == x.m_document && blockNumber() == x.blockNumber(); }
	bool	operator!=(const TextBlock &x) const
	{ return !this->operator==(x); }
	bool	operator<(const TextBlock &x) const
	{ return m_document == x.m_document && blockNumber() < x.blockNumber(); }

public:
	TextBlock	next() const;
	TextBlock	prev() const;

private:
	TextDocument	*m_document;
	TextBlockData	m_data;
};

//----------------------------------------------------------------------
//	����̓u���b�N�T�C�Y�݂̂����A�ߖ����� �t���O�ނ�ǉ�����
struct TextBlockItem
{
	size_t		m_size;		//	�u���b�N�������T�C�Y
public:
	TextBlockItem(size_t size = 0) : m_size(size) {}
};
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
	bool	isMatch(index_t, cuchar *, cuchar *, ushort) const;		//	�P����r�֐�
	bool	isMatch(index_t, cuchar *, cuchar *) const;		//	�P����r�֐�
	bool	isMatchIgnoreCase(index_t, cuchar *, cuchar *) const;		//	�P����r�֐�
	uchar	charEncoding() const { return m_charEncoding; }
	bool	withBOM() const { return m_withBOM; }
	TextBlockData blockData() const { return m_blockData; }

	TextBlockData	findBlockData(index_t position) const;
	TextBlockData	nextBlockData(TextBlockData d) const
	{ return TextBlockData(d.m_index + 1, d.m_position + m_blocks[d.m_index].m_size); }
	TextBlockData	prevBlockData(TextBlockData d) const
	{
		if( !d.m_index )
			return TextBlockData(INVALID_INDEX, 0);
		else {
			size_t sz = m_blocks[d.m_index - 1].m_size;
			return TextBlockData(d.m_index - 1, d.m_position - m_blocks[d.m_index - 1].m_size);
		}
	}

public:
	bool	canUndo() const { return m_undoMgr.canUndo(); };
	bool	canRedo() const { return m_undoMgr.canRedo(); };

public:
	void	init();
	void	clear() { init(); }

	void	setModified(bool b) { m_modified = b; }
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
	TextBlock	findBlock(index_t) const;
	index_t		findBlockIndex(index_t position, index_t *pBlockPos = 0) const;
	TextBlock	findBlockByNumber(index_t) const;		//	�u���b�N�ԍ��i0..*�j����u���b�N���擾
	TextBlock	findBlockByNumberRaw(index_t) const;		//	�u���b�N�ԍ��i0..*�j����u���b�N���擾

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
	void	do_erase(index_t, index_t, ushort=0);				//	undo/redo �Ή���
	void	do_replace(index_t, index_t, const QString &);		//	undo/redo �Ή���

	size_t	insertText(TextCursor&, const QString &);
	void	deleteChar(TextCursor&);
	void	deletePreviousChar(TextCursor&);

	void	doUndo(index_t &pos, index_t &); //{ m_undoMgr.doUndo(this, pos); }
	void	doRedo(index_t &pos, index_t &); //{ m_undoMgr.doRedo(this, pos); }
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

	TextCursor	find(const QString &, index_t = 0, ushort=0);
	TextCursor	find(const QString &, const TextCursor &, ushort=0);
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
	mutable TextBlockData	m_blockData;			//	�J�����g�u���b�N���
	//index_t		m_blockIndex;		//	�J�����g�u���b�N���
	//index_t		m_blockPosition;	//	�J�����g�u���b�N���
	//CBuffer_GV	m_buffer;		//	����UTF-8�ȃo�b�t�@
	GVUndoMgr	m_undoMgr;
	//boost::object_pool<GVUndoItem>	m_pool_undoItem;

	friend void test_TextDocument();
};

#endif // TEXTDOCUMENT_H
