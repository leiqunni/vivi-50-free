//----------------------------------------------------------------------
//
//			File:			"TextDocument.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextDocument クラス宣言
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

*/

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <QObject>
#include	<vector>
//#include	<boost/pool/object_pool.hpp>
#include	"gap_vector.h"
//#include	"buffer.h"
#include	"textBlock.h"

typedef unsigned char uchar;
typedef const unsigned char cuchar;

//#define		INVALID_INDEX		0xffffffff
class DocCursor;

//typedef std::gap_vector<uchar> GV_utf8;

//#define		BLOCK_HAS_OFFSET	0
#define		BLOCK_HAS_SIZE		1
#if 0
enum {
	CT_EOF = 0,
	CT_NEWLINE,
	CT_SPACE,
	CT_ALNUM,		//	半角英数字
	CT_HANKANA,		//	半角カナ
	CT_SYM,			//	半角シンボル
	//CT_DBSPACE,		//	全角空白
	CT_HIRA,		//	全角ひらがな
	CT_KANA,		//	全角カタカナ
	CT_DBALNUM,		//	全角英数字
	CT_KANJI,		//	漢字
	//CT_SYM,			//	全角記号
	CT_CONT,		//	繰り返し記号（々など）
	CT_GREEK,		//	ギリシャ文字
	CT_ALPHA_EX,	//	その他の国の文字
	CT_OTHER,		//	その他文字
};
#endif

class TextDocument;
class DocBlock;

enum {
	EOL_UNKNOWN = 0,
	EOL_CRLF,
	EOL_CR,
	EOL_LF,
};

//----------------------------------------------------------------------
//	undo/redo 文字列の格納にヒープを用い、undoItem クラスは継承をやめ、type でディスパッチする
//#define		UNDOMGR_USE_HEAP			1

enum {
	GVUNDOITEM_UNDO_MF_OFF = 0x0001,	//	Undo でモディファイフラグがＯＮからＯＦＦに
	GVUNDOITEM_REDO_MF_OFF = 0x0002,	//	Redo でモディファイフラグがＯＮからＯＦＦに
	GVUNDOITEM_DO_NOT_MERGE = 0x0004,	//	マージ不可
	//GVUNDOITEM_MADELINE = 0x0004,
	GVUNDOITEM_CUR_TAIL = 0x0008,		//	削除の時、カーソルが最後にあった
	GVUNDOITEM_SHIFT_LEFT = 0x0010,
	GVUNDOITEM_SHIFT_RIGHT = 0x0000,
	//GVUNDOITEM_CONCAT_LINE = 0x0020,	//	挿入文字列連結可能
	//GVUNDOITEM_CONCAT_ALL = 0x0040,		//	挿入文字列連結可能
	GVUNDOITEM_DELETE = 0x0080,			//	or BackSpace
	GVUNDOITEM_MARK_POS = 0x0100,		//	CUndoItemMarkPos かどうか
	GVUNDOITEM_BLOCK = 0x0200,			//	CUndoItemBlock かどうか
	GVUNDOITEM_SAVED = 0x0400,			//	ドキュメントが保存された
	GVUNDOITEM_DIFF_RESTORE = 0x0800,		//	diff で相手ファイルにあわせる
	GVUNDOITEM_REPTEXT = 0x1000,		//	文字列の置換を行った
	GVUNDOITEM_TRANSLATE = 0x2000,		//	コード変換の場合
	GVUNDOITEM_REDRAW = 0x4000,			//	:m の場合の様に強制的にリドローする
	//GVUNDOITEM_INSTEXT = 0x8000,		//	文字列の挿入を行った
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
	bool	m_beforeSave:1;		//	保存前の undo/redo item
	short	m_flags;
	index_t	m_first;
	index_t	m_last;				//	挿入・削除・置換後範囲
	index_t	m_last2;			//	置換前の範囲
	index_t	m_hp_ix;
	index_t	m_rhp_ix;			//	redo 用ヒープインデックス
	//size_t	m_hpSize;			//	ヒープに格納されているデータサイズ（バイト数）
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
	uint	m_current;		//	オブジェクトを次に入れる位置（0..*）
							//	通常は m_item.size() と同値だが、undo が実行されるとデクリメントされる
	uint	m_undoBlockLevel;
	bool	m_toSetBlockFlag;			//	次の push_back で block フラグを立てる
	TextDocument	*m_document;
	std::vector<GVUndoItem>	m_items;
	std::vector<uchar>	m_heap;			//	undo のための文字列を格納するヒープ
	std::vector<uchar>	m_redoHeap;		//	redo のための文字列を格納するヒープ
public:
	GVUndoMgr(TextDocument *document)
		: m_document(document), m_current(0)
	{
		m_undoBlockLevel = 0;
		m_toSetBlockFlag = false;
		m_heap.push_back(1);			//	最初のデータインデックスを 1 にするためのダミー
		m_redoHeap.push_back(1);
	}
	~GVUndoMgr()
	{
		//std::cout << "m_items.size() = " << m_items.size() << "\n";
	}
public:
	bool	canUndo() const { return m_current != 0; };
	bool	canRedo() const { return m_current < m_items.size(); };
	//bool	canMerge(const GVUndoItem &) const;
public:
	TextDocument	*document()	{ return m_document; }
	void	openUndoBlock();		//	undo block レベル += 1
	void	closeUndoBlock();		//	undo block レベル -= 1
	void	resetUndoBlock();		//	undo block レベル = 0
	//void	push_back(GVUndoItem *ptr, bool = false);
	void	push_back(const GVUndoItem &, bool = false);
	bool	doMergeIfPossible(const GVUndoItem &);
	void	setDoNotMerge();		//	マージ不可フラグON

	//	データをヒープに追加
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
	void	resetModifiedFlags();	//	ドキュメントが保存された時にコールされる
	bool	doUndo(/*TextDocument*,*/ index_t&, index_t&);
	bool	doRedo(/*TextDocument*,*/ index_t&, index_t&);
};
//----------------------------------------------------------------------

#if 0
class DocBlock
{
public:
	//DocBlock(TextDocument *document, index_t blockNumber)
	//	: m_document(document), m_blockNumber(blockNumber)
	//	{}
	DocBlock(TextDocument *document, index_t blockNumber, index_t blockPosition)
		: m_document(document), m_data(BlockData(blockNumber, blockPosition))
		{}
	DocBlock(TextDocument *document, BlockData block)
		: m_document(document), m_data(block)
		{}
	DocBlock(const DocBlock &x)
		: m_document(x.m_document)
#if	BLOCK_HAS_SIZE
		, m_data(x.m_data)
#endif
		{}
	~DocBlock() {}

public:
	size_t		size() const;		//	改行を含めたコード長
	size_t		length() const { return size(); }
	index_t		EOLOffset() const;
	size_t		newlineLength() const;		//	改行部分のバイト数を返す
	bool		isValid() const { return m_document != 0 && blockNumber() != INVALID_INDEX; }
	index_t		index() const { return m_data.m_index; }
	index_t		blockNumber() const { return m_data.m_index; }
	index_t		position() const;	// { return isValid() ? m_document->blockPosition(m_index) : 0; }
	BlockData	data() const { return m_data; }
	QString		text() const;
	int			charsCount(index_t) const;		//	行頭から指定位置までの文字数を返す

	bool	operator==(const DocBlock &x) const
	{ return m_document == x.m_document && blockNumber() == x.blockNumber(); }
	bool	operator!=(const DocBlock &x) const
	{ return !this->operator==(x); }
	bool	operator<(const DocBlock &x) const
	{ return m_document == x.m_document && blockNumber() < x.blockNumber(); }

public:
	DocBlock	next() const;
	DocBlock	prev() const;

private:
	TextDocument	*m_document;
	BlockData	m_data;
};
#endif

//----------------------------------------------------------------------
//	現状はブロックサイズのみだが、近未来に フラグ類を追加する
struct DocBlockItem
{
	size_t		m_size;		//	ブロック内文字サイズ
public:
	DocBlockItem(size_t size = 0) : m_size(size) {}
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
	uchar	at(index_t ix) const { return m_buffer[ix]; }
	QChar	charAt(index_t ix) const;
	QString	toPlainText() const;
	QString	EOLText() const;
	bool	isMatch(index_t, cuchar *, cuchar *, ushort) const;		//	単純比較関数
	bool	isMatch(index_t, cuchar *, cuchar *) const;		//	単純比較関数
	bool	isMatchIgnoreCase(index_t, cuchar *, cuchar *) const;		//	単純比較関数
	uchar	charEncoding() const { return m_charEncoding; }
	bool	withBOM() const { return m_withBOM; }
	BlockData cacheBlockData() const { return m_cacheBlockData; }

	BlockData	findBlockData(index_t position) const;
	BlockData	nextBlockData(BlockData d) const
	{
		index_t pos = d.m_index < m_blocks.size() ? d.m_position + m_blocks[d.m_index].m_size : size() + 1;
		return BlockData(d.m_index + 1, pos);
	}
	BlockData	prevBlockData(BlockData d) const
	{
		if( !d.m_index )
			return BlockData(INVALID_INDEX, 0);
		else {
			//size_t sz = m_blocks[d.m_index - 1].m_size;
			return BlockData(d.m_index - 1, d.m_position - m_blocks[d.m_index - 1].m_size);
		}
	}
	index_t	markedPos(uchar uch) const;		//	'a' <= uch <= 'z' の場合のみ有効

public:
	bool	canUndo() const { return m_undoMgr->canUndo(); };
	bool	canRedo() const { return m_undoMgr->canRedo(); };

public:
	void	init();
	void	clear() { init(); }
	void	setEOLCode();
	void	setModified(bool b) { m_modified = b; }
	void	setFullPath(const QString &fullPath) { m_fullPath = fullPath; }
	void	openUndoBlock() { m_undoMgr->openUndoBlock(); }
	void	closeUndoBlock() { m_undoMgr->closeUndoBlock(); }

#if BLOCK_HAS_SIZE
	DocBlock	firstBlock() { return DocBlock(this, 0, 0); }
	DocBlock	lastBlock()
	{
		return DocBlock((TextDocument*)this, blockCount() - 1,
						size() - m_blocks[blockCount() - 1].m_size);
	}
#else
	DocBlock	firstBlock() { return DocBlock(this, 0); }
	DocBlock	lastBlock() { return DocBlock(this, blockCount() - 1); }
#endif
	DocBlock	findBlock(index_t) const;
	index_t		findBlockIndex(index_t position, index_t *pBlockPos = 0) const;
	DocBlock	findBlockByNumber(index_t) const;		//	ブロック番号（0..*）からブロックを取得
	DocBlock	findBlockByNumberRaw(index_t) const;		//	ブロック番号（0..*）からブロックを取得

	void	erase(index_t, index_t);
	void	erase(index_t, BlockData, index_t);
	void	insert(index_t, const QString &);
	void	insert(index_t ix, cuchar *first, cuchar *last);
	void	insert(index_t, BlockData, const QString &);
	void	insert(index_t ix, BlockData, cuchar *first, cuchar *last);
	void	setPlainText(const QString &);
	//void	append(const QByteArray &);		//	UTF-8 配列
	//void	updateBlocksAtInsert(index_t, size_t);
	void	updateBlocksAtInsert(index_t, BlockData, size_t);
	//void	updateBlocksAtErase(index_t, index_t);
	void	updateBlocksAtErase(index_t, BlockData, index_t);

	void	do_insert(index_t, const QString &);	//	undo/redo 対応版
	void	do_erase(index_t, index_t, ushort=0);				//	undo/redo 対応版
	void	do_replace(index_t, index_t, const QString &);		//	undo/redo 対応版

	int		insertText(DocCursor&, const QString &, bool = false);
	size_t	deleteChar(DocCursor&);
	size_t	deletePreviousChar(DocCursor&);

	void	doUndo(index_t &pos, index_t &); //{ m_undoMgr.doUndo(this, pos); }
	void	doRedo(index_t &pos, index_t &); //{ m_undoMgr.doRedo(this, pos); }
	void	setDoNotMergeUndoItem();

	DocCursor	find(const QString &, index_t = 0, ushort=0);
	DocCursor	find(const QString &, const DocCursor &, ushort=0);
	DocCursor	find(const QByteArray &, index_t = 0, ushort=0);
	bool	isMatched(const QString &, const DocCursor &, ushort=0);
	void	doReplaceAll(const QString &, ushort, const QString &);
	void	setCharEncodeing(uchar ce) { m_charEncoding = ce; }
	void	setWithBOM(bool b) { m_withBOM = b; }

	void	updateMarkedPos(index_t pos, int d);
	void	setMarkedPos(uchar uch, index_t);		//	'a' <= uch <= 'z'

public:
	std::gap_vector<uchar>::iterator	begin() { return m_buffer.begin(); }
	std::gap_vector<uchar>::iterator	end() { return m_buffer.end(); }

public:
	//void	do_unitTest();		//	単体テスト

signals:
	void contentsChange( index_t position, size_t charsRemoved, size_t charsAdded );
	void contentsChanged();
	void blockCountChanged();
	void showMessage(const QString &);

protected:
	void	buildBlocks();

private:
	index_t	findBlockIndex(index_t first, index_t last, index_t val) const;

private:
	QString	m_fullPath;
	uchar	m_charEncoding;
	bool	m_withBOM;
	bool	m_modified;
	uchar	m_EOLCode;
	mutable std::gap_vector<uchar>	m_buffer;
	//mutable std::gap_vector<size_t>	m_blocks;		//	ブロックサイズ配列
	mutable std::gap_vector<DocBlockItem>	m_blocks;		//	ブロック配列
	mutable BlockData	m_cacheBlockData;			//	カレントブロック情報
	//index_t		m_blockIndex;		//	カレントブロック情報
	//index_t		m_blockPosition;	//	カレントブロック情報
	//CBuffer_GV	m_buffer;		//	内部UTF-8なバッファ
	GVUndoMgr	*m_undoMgr;
	//boost::object_pool<GVUndoItem>	m_pool_undoItem;
	index_t		m_markedPos[26];		//	a-z マーク位置

	friend void test_TextDocument();
};

#endif // TEXTDOCUMENT_H
