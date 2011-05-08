//----------------------------------------------------------------------
//
//			File:			"TextView.h"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextView �N���X�錾
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

#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#define		LAIDOUT_BLOCKS_MGR		1
#define		LAZY_LAIDOUT			0

#include <deque>
#include <QAbstractScrollArea>
#include	"gap_vector.h"
#include	"textBlock.h"
#include	"textCursor.h"

class ViEngine;
class TextView;
class ViewCursor;
class TextDocument;
class DocCursor;
class DocBlock;
class LaidoutBlocksMgr;
//class QElapsedTimer;
class QTimer;

struct ViewLine
{
public:
	index_t	m_position;		//	�s�������ʒu
	index_t	m_blockIndex;	//	�u���b�N�C���f�b�N�X

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
	ViewBlock	findBlockByNumber(index_t) const;		//	�u���b�N�ԍ��i0..*�j����u���b�N���擾
	BlockData	findBlockData(index_t position) const;
	BlockData	nextBlockData(BlockData d) const
	{
		return BlockData(d.m_index + 1, d.m_position + blockSize(d.m_index));
	}
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
#if !LAIDOUT_BLOCKS_MGR
	size_t	firstUnlayoutedBlockCount() const { return m_firstUnlayoutedBlockCount; }
	size_t	layoutedDocBlockCount() const { return m_layoutedDocBlockCount; }
#endif
#if 0
	index_t	firstViewLine() const { return m_firstViewLine; }
	index_t	lastViewLine() const { return m_lastViewLine; }
#endif
	int		charCountToX(const QString &, int) const;		//	��Q�����͕�����
	int		xToCharCount(const QString &, int) const;		//	�����W�ɑΉ����镶������Ԃ�
	index_t	movePositionByCharCount(index_t, int n) const;	//	n �������ړ�
	BlockData cacheBlockData() const { return m_cacheBlockData; }
#if LAIDOUT_BLOCKS_MGR
	LaidoutBlocksMgr	*lbMgr() const { return m_lbMgr; }
#endif
	const ViewBlock *firstVisibleBlockPtr() const;
	const ViewBlock *lastVisibleBlockPtr() const;

public:
	TextDocument	*document() { return m_document; }
	void	setViEngine(ViEngine *);
	void	doJump(int lineNum);
	void	doVertScroll(int);
	void	setOverwriteMode(bool);
	void	onFontChanged();

public:
	ViewCursor	textCursor() { return *m_textCursor; }
	ViewBlock	firstBlock() const;
	ViewBlock	lastBlock() const;
	ViewBlock	findBlock(index_t) const;

	void	setTextCursor(const ViewCursor &cur);
	void	setLineBreakMode(bool b) { onLineBreak(b); }

	void	doDelete(int, int);
	void	doOpenLine(bool next = true);		//	next = false �Ȃ�΁A���O�ɍs�I�[�v��
	void	doUndo(int n = 1);
	void	doRedo(int n = 1);

	void	deleteChar();
	void	deletePreviousChar();
	void	insertText(const QString &, bool = false);
	int		insertText(ViewCursor &, const QString &);
	size_t	deleteChar(ViewCursor &);
	size_t	deletePreviousChar(ViewCursor &);
	void	buildBlocks() { buildBlocks(firstBlock(), 0, 0); }

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
	int		textBlockToY(const ViewBlock&) const;		//	�����X�N���[�����l�������Ablock Y���W��Ԃ�
														//	��ʊO�̏ꍇ�� -1 ��Ԃ�
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
	void	drawCursor(QPainter &painter, const ViewCursor &, ViewBlock &,
						const QString &text, int y, const QColor &color);
	void	doPaint();
	void	updateLineNumberAreaSize();
	void	updateScrollBarData();
	void	drawLineNumbers();
	void	resetCursorBlinkTimer();
	void	clearMultiCursor() { m_multiCursor.clear(); }
	void	addToMultiCursor();
	void	addToMultiCursor(const ViewCursor &cur) { m_multiCursor.push_back(cur); };
	void	getAllCursor(std::vector<ViewCursor*> &);
	void	ensureBlockLayout();
	void	updateBlocks();
	void	clearBlocks();
	void	eraseBlocks(index_t, index_t, size_t);
	void	buildBlocks(ViewBlock, int ht = 0, index_t = 0);
	//void	reLayoutBlocks(DocBlock, index_t lastPosition, index_t vbIndex);
	void	reLayoutBlocksUntillDocBlockNumber(DocBlock, index_t lastBlockNumber, index_t vbIndex);
	void	layoutText(std::vector<size_t> &, const DocBlock &, int wd, int tabWidth);
	void	layoutText(std::vector<size_t> &, const QString &, index_t, int wd, int tabWidth);
	void	getReLayoutRange(ViewCursor cur,
								DocBlock &block,		//	�ă��C�A�E�g�J�n�u���b�N
								index_t &lastPosition,	//	�ă��C�A�E�g�I���ʒu [block, last)
								index_t &firstViewBlockNumber);	//	�ă��C�A�E�g�J�n�r���[�u���b�N�ԍ�
								//index_t &lastViewBlockIndex);	//	�ă��C�A�E�g�I���ʒu [first, last)
	void	getReLayoutRangeByBlockNumber(ViewCursor cur,
								DocBlock &block,		//	�ă��C�A�E�g�J�n�u���b�N
								index_t &lastBlockNumber,	//	�ă��C�A�E�g�I���ʒu [block, last)
								index_t &firstViewBlockNumber);	//	�ă��C�A�E�g�J�n�r���[�u���b�N�ԍ�

	void	resetCursorBlink();
    void	removeOverlappedCursor();
    ViewBlock	firstVisibleBlock() const;
    void	ensureCursorVisible();
	int		lineNumberLength() const;			//	�ő�s�ԍ�����

protected slots:
	void	layout100Blocks();				//	�V����100�s���C�A�E�g����
	void	onBlockCountChanged();
	void	onTimer();

signals:
	void	doLayout100Blocks();			//	����100�s�����C�A�E�g
	void	printBuffer();
	void	showMessage(const QString &);

private:
	bool	m_mouseCaptured;
	bool	m_toDeleteIMEPreeditText;
	bool	m_drawCursor;
	bool	m_lineBreakMode;					//	�E�[�Ő܂�Ԃ�
	bool	m_lineBreaking;						//	�܂�Ԃ�������
	ViEngine	*m_viEngine;
	ViewCursor	m_viewTextCursor;
	std::vector<ViewCursor>	m_multiCursor;		//	���J�[�\���Aposition ���L�[�ɏ����\�[�g�ς݂Ƃ���
													//	���͏����Ɖ��肵�� std::vector ��p����
	ViewCursor	*m_preeditPosCursor;
	QString	m_preeditString;
	TextDocument	*m_document;
	ViewCursor	*m_textCursor;					//	�r���[�p�J�[�\��
	mutable bool		m_validFirstVisibleBlock;		//	m_firstVisibleBlock �L��
	mutable size_t		m_firstVisibleDocBlockNumber;
	mutable BlockData	m_firstVisibleBlock;			//	�\���J�n�s
	int		m_viewportWidth;
	QWidget	*m_lineNumberArea;
	int		m_lineNumberAreaWidth;
	int		m_lineNumberWidth;
	//int		m_lineNumberNDigits;		//	����
	QTimer	*m_timer;					//	�^�C�}�[�I�u�W�F�N�g
	//mutable std::gap_vector<ViewTextBlockItem>	m_blocks;		//	�u���b�N�z��
	mutable BlockData	m_cacheBlockData;			//	�J�����g�u���b�N���

#if LAIDOUT_BLOCKS_MGR
	mutable LaidoutBlocksMgr	*m_lbMgr;
#else
	size_t	m_firstUnlayoutedBlockCount;		//	�O�������C�A�E�g������DocBlock��
	size_t	m_layoutedDocBlockCount;				//	���C�A�E�g�ςݍs��DocBlock��
	mutable std::gap_vector<uint>	m_blockSize;	//	���C�A�E�g�ςݍs�̍s��
#endif
#if 0
	size_t	m_firstViewLine;
	size_t	m_lastViewLine;
	mutable std::deque<ViewLine>	m_viewLines;
#endif

	friend void test_TextView();
	friend void test_LaidoutBlock();
};

#endif // PLAINTEXTEDIT_H
