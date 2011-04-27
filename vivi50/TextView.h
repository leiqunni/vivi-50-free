//----------------------------------------------------------------------
//
//			File:			"TextView.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextView クラス宣言
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

#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <deque>
#include <QAbstractScrollArea>
#include	"gap_vector.h"
#include	"textBlock.h"
#include	"textCursor.h"

class TextView;
class ViewCursor;
class TextDocument;
class DocCursor;
class DocBlock;
//class QElapsedTimer;
class QTimer;

struct ViewLine
{
public:
	index_t	m_position;		//	行頭文字位置
	index_t	m_blockIndex;	//	ブロックインデックス

public:
	ViewLine(index_t position, index_t blockIndex)
		: m_position(position), m_blockIndex(blockIndex)
		{}
};


#if 0
class ViewBlock : public DocBlock;
{
private:
	TextView	*m_view;
};
#endif

class TextView : public QAbstractScrollArea
{
	Q_OBJECT

public:
	TextView(QWidget *parent = 0);
	~TextView();

public:
	uchar	charEncoding() const;
	bool	withBOM() const;
	bool	isModified() const;
	bool	hasMultiCursor() const { return !m_multiCursor.empty(); }
	bool	lineBreakMode() const { return m_lineBreakMode; }
	QString	toPlainText() const;
	const TextDocument	*document() const { return m_document; }
	//size_t blockCount() const { return m_blocks.size(); }
	BlockData	docBlockData(BlockData) const;
	DocBlock	docBlock(BlockData) const;
	ViewBlock	findBlockByNumber(index_t) const;		//	ブロック番号（0..*）からブロックを取得
	BlockData	findBlockData(index_t position) const;
	BlockData	nextBlockData(BlockData d) const
	{ return BlockData(d.m_index + 1, d.m_position + blockSize(d.m_index)); }
	BlockData	prevBlockData(BlockData d) const
	{
		if( !d.m_index )
			return BlockData(INVALID_INDEX, 0);
		else {
			return BlockData(d.m_index - 1, d.m_position - blockSize(d.m_index - 1));
		}
	}
	size_t	size() const;	// { return document()->size(); }
	bool	isLayoutedDocBlock(index_t ix) const;
	bool	isLayoutedViewBlock(index_t ix) const;
	size_t	blockSize(index_t ix) const;	// { return m_blocks[ix].m_size; }
	size_t	blockCount() const;
	size_t	firstUnlayoutedBlockCount() const { return m_firstUnlayoutedBlockCount; }
	size_t	layoutedBlockCount() const { return m_layoutedBlockCount; }
#if 0
	index_t	firstViewLine() const { return m_firstViewLine; }
	index_t	lastViewLine() const { return m_lastViewLine; }
#endif
	int		charCountToX(const QString &, int) const;		//	第２引数は文字数
	int		xToCharCount(const QString &, int) const;		//	ｘ座標に対応する文字数を返す
	index_t	movePositionByCharCount(index_t, int n) const;	//	n 文字分移動
	BlockData blockData() const { return m_blockData; }

public:
	TextDocument	*document() { return m_document; }
	void	doJump(int lineNum);
	void	onFontChanged();

public:
	ViewBlock	firstBlock();
	ViewBlock	lastBlock();
	ViewBlock	findBlock(index_t) const;

	void	setLineBreakMode(bool b) { onLineBreak(b); }

	void	deleteChar();
	void	deletePreviousChar();
	void	insertText(const QString &, bool = false);
	int		insertText(ViewCursor &, const QString &);
	size_t	deleteChar(ViewCursor &);
	size_t	deletePreviousChar(ViewCursor &);

public slots:
	void	copy();
	void	cut();
	void	paste();
	void	undo();
	void	redo();
	void	selectAll();
	void	replace();
	void	find();
	void	findNext();
	void	findPrev();
	void	findCurWord();
	void	doFindNext(const QString &, ushort);
	void	isMatched(bool &, const QString &, ushort);
	void	doReplace(const QString &);
	void	doReplaceAll(const QString &, ushort, const QString &);
	void	setFontPointSize(int);
	void	setFontFamily(const QString &);
	void	makeFontBigger(bool);
	void	onLineBreak(bool);

protected:
	ViewBlock	yToTextBlock(int) const;
	int		textBlockToY(const DocBlock&) const;		//	垂直スクロールを考慮した、block Y座標を返す
														//	画面外の場合は -1 を返す
protected:
	bool	eventFilter(QObject *obj, QEvent *event);
	bool	event ( QEvent * event );
	void	paintEvent(QPaintEvent * event);
    void	keyPressEvent ( QKeyEvent * keyEvent );
    void	inputMethodEvent ( QInputMethodEvent * event );
	void	wheelEvent ( QWheelEvent * event );
    void	focusInEvent ( QFocusEvent * event );
    void	resizeEvent ( QResizeEvent * event );
    void	mousePressEvent ( QMouseEvent * event );
    void	mouseReleaseEvent ( QMouseEvent * event );
    void	mouseMoveEvent ( QMouseEvent * event );
    void	mouseDoubleClickEvent ( QMouseEvent * event );
    QVariant	inputMethodQuery ( Qt::InputMethodQuery query ) const;
	void	updateLineNumberAreaSize();
	void	updateScrollBarData();
	void	drawLineNumbers();
	void	setTextCursor(const ViewCursor &cur) { *m_textCursor = cur; }
	void	resetCursorBlinkTimer();
	void	clearMultiCursor() { m_multiCursor.clear(); }
	void	addToMultiCursor(const ViewCursor &cur) { m_multiCursor.push_back(cur); }
	void	getAllCursor(std::vector<ViewCursor*> &);
	void	ensureBlockLayout();
	void	updateBlocks();
	void	clearBlocks();
	void	eraseBlocks(index_t, index_t);
	void	buildBlocks(DocBlock, int wd, int ht, index_t = 0);
	void	reLayoutBlocks(DocBlock, index_t lastPosition, index_t vbIndex);
	void	layoutText(std::vector<size_t> &, const DocBlock &, int wd, int tabWidth);

	void	resetCursorBlink();
    void	removeOverlappedCursor();
    ViewBlock	firstVisibleBlock() const;
    void	ensureCursorVisible();
	int		lineNumberLength() const;			//	最大行番号桁数

protected slots:
	void	onBlockCountChanged();
	void	onTimer();

signals:
	void	showMessage(const QString &);

private:
	bool	m_mouseCaptured;
	bool	m_toDeleteIMEPreeditText;
	bool	m_drawCursor;
	bool	m_lineBreakMode;					//	右端で折り返し
	ViewCursor	m_viewTextCursor;
	std::vector<ViewCursor>	m_multiCursor;		//	副カーソル、position をキーに昇順ソート済みとする
													//	個数は少数と仮定して std::vector を用いる
	DocCursor	*m_preeditPosCursor;
	QString	m_preeditString;
	TextDocument	*m_document;
	ViewCursor	*m_textCursor;		//	ビュー用カーソル
	int		m_viewportWidth;
	QWidget	*m_lineNumberArea;
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
	//int		m_lineNumberNDigits;		//	桁数
	QTimer	*m_timer;					//	タイマーオブジェクト
	//mutable std::gap_vector<ViewTextBlockItem>	m_blocks;		//	ブロック配列
	mutable BlockData	m_blockData;			//	カレントブロック情報

	size_t	m_firstUnlayoutedBlockCount;		//	前半未レイアウト部分のDocBlock数
	size_t	m_layoutedBlockCount;				//	レイアウト済み行のDocBlock数
	mutable std::gap_vector<uint>	m_blockSize;	//	レイアウト済み行の行長
#if 0
	size_t	m_firstViewLine;
	size_t	m_lastViewLine;
	mutable std::deque<ViewLine>	m_viewLines;
#endif

	friend void test_TextView();
};

#endif // PLAINTEXTEDIT_H
