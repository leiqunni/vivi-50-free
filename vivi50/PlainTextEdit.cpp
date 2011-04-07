//----------------------------------------------------------------------
//
//			File:			"PlainTextEdit.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	PlainTextEdit クラス実装
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

#include <QtGui>
#include "PlainTextEdit.h"
#include	"TextDocument.h"
#include	<QDebug>

#define		MARGIN_LEFT		4
//#define		LINE_HEIGHT		20

PlainTextEdit::PlainTextEdit(QWidget *parent)
	: QAbstractScrollArea(parent)
{

	m_document = new TextDocument();
	m_textCursor = new TextCursor(m_document);
	connect(m_document, SIGNAL(blockCountChanged()), this, SLOT(onBlockCountChanged()));

	m_lineNumberArea = new QWidget(this);
	m_lineNumberArea->installEventFilter(this);
	setFontPointSize(11);
	//onFontChanged();

	//m_document->setPlainText(QString("LINE-1\nLINE-2\nLINE-3\n"));
}

PlainTextEdit::~PlainTextEdit()
{
	delete m_document;
	delete m_textCursor;
}
void PlainTextEdit::onFontChanged()
{
	//setTabStopWidth(fontMetrics().width('>') * 4);		//	tab 4
	m_lineNumberWidth = fontMetrics().width('8') * 6;
	m_lineNumberAreaWidth = fontMetrics().width('8') * 8;
	setViewportMargins(m_lineNumberAreaWidth, 0, 0, 0);
	updateLineNumberAreaSize();
}
void PlainTextEdit::onBlockCountChanged()
{
	QFontMetrics fm = fontMetrics();
	QSize areaSize = viewport()->size();
	//QSize  widgetSize = widget->size();

	verticalScrollBar()->setPageStep(m_document->blockCount() /** fm.lineSpacing()*/);
	verticalScrollBar()->setSingleStep(1 /*fm.lineSpacing()*/);
	//horizontalScrollBar()->setPageStep(widgetSize.width());
	verticalScrollBar()->setRange(0, m_document->blockCount() /** fm.lineSpacing()*/ - areaSize.height() / fm.lineSpacing());
	//horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
	//updateWidgetPosition();

	m_lineNumberArea->update();
}
void PlainTextEdit::focusInEvent ( QFocusEvent * event )
{
	QAbstractScrollArea::focusInEvent( event );
	const QString fullPath = document()->fullPath();
	if( !fullPath.isEmpty() ) {
		QDir dir(fullPath);
		dir.cdUp();
		QDir::setCurrent(dir.path());
	}
}

int PlainTextEdit::offsetToX(const QString &text, int offset) const
{
	offset = qMin(offset, text.length());
	const QFontMetrics fm = fontMetrics();
	const int spaceWidth = fm.width(QChar(' '));
	const int tabWidth = spaceWidth * 4;		//	とりあえず空白4文字分に固定
	int x = 0;
	for(int ix = 0; ix < offset; ) {
		if( text[ix] == ' ' ) {
			//	boundingRect(text)：text の最後が空白の場合に期待する値を返さないため
			++ix;
			x += spaceWidth;
		} else if( text[ix] == '\t' ) {
			++ix;
			x = (x / tabWidth + 1) * tabWidth;
		} else {
			int first = ix;
			while( ix < offset && text[ix] != '\t' && text[ix] != ' ' )
				++ix;
			const QString buf = text.mid(first, ix - first);
			x += fm.boundingRect(buf).width();
		}
	}
	return x;
}

void PlainTextEdit::paintEvent(QPaintEvent * event)
{
	//qDebug() << verticalScrollBar()->value();

	QWidget *vp = viewport();
	QRect vr = vp->rect();
	QPainter painter(vp);

	QFontMetrics fm = fontMetrics();
	const int spaceWidth = fm.width(QChar(' '));
	const int tabWidth = spaceWidth * 4;		//	とりあえず空白4文字分に固定

	index_t selFirst = 0, selLast = 0;	//	選択範囲、first < last
	if( m_textCursor->hasSelection() ) {
		if( m_textCursor->position() < m_textCursor->anchor() ) {
			selFirst = m_textCursor->position();
			selLast = m_textCursor->anchor();
		} else {
			selFirst = m_textCursor->anchor();
			selLast = m_textCursor->position();
		}
	}
	index_t lastBlockNumber = m_document->lastBlock().blockNumber();
	int y = 0;
	TextBlock block = m_document->findBlockByNumber(verticalScrollBar()->value() /*/ fm.lineSpacing()*/);
	//TextBlock block = m_document->firstBlock();
	while( y < vr.height() && block.isValid() ) {
		const QString text = block.text();
		index_t nextBlockPosition = block.position() + m_document->blockSize(block.index());
		if( m_textCursor->hasSelection() &&
			selFirst < nextBlockPosition && selLast > block.position() )
		{
			//	block が選択範囲内にある場合
			int x1 = offsetToX(text, block.charsCount(qMax(block.position(), selFirst)));
			int x2 = offsetToX(text, block.charsCount(qMin(nextBlockPosition, selLast)));
			painter.fillRect(QRect(x1 + MARGIN_LEFT + 1, y+2, x2 - x1, fm.height()), Qt::lightGray);
		}
		if( m_textCursor->block() == block) {		//	カーソルがブロック内にある場合
			const int offset = qMin(block.charsCount(m_textCursor->position()),
										text.length());
			int x = offsetToX(text, offset);
			painter.fillRect(QRect(x + MARGIN_LEFT + 1, y+2, 2, fm.height()), Qt::red);
		}
		//painter.drawText(MARGIN_LEFT, y + fm.ascent(), text);
		int x = 0;
		int ix = 0;
		while( ix < text.length() ) {
			if( text[ix] == '\t' ) {
				painter.setPen(Qt::lightGray);
				painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), ">");
				++ix;
				x = (x / tabWidth + 1) * tabWidth;
			} else {
				int first = ix;
				while( ix < text.length() && text[ix] != '\t' )
					++ix;
				const QString buf = text.mid(first, ix - first);
				painter.setPen(Qt::black);
				painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), buf);
				x += fm.boundingRect(buf).width();
			}
		}
		if( block.blockNumber() == lastBlockNumber ) {
			painter.setPen(Qt::blue);
			const int x = offsetToX(text, text.length());
			painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), "[EOF]");
			break;
		}
		block = block.next();
		y += fm.lineSpacing();
	}
	m_lineNumberArea->update();
}
TextBlock PlainTextEdit::firstVisibleBlock()
{
	//QFontMetrics fm = fontMetrics();
	return m_document->findBlockByNumber(verticalScrollBar()->value() /*/ fm.lineSpacing()*/);
}
void PlainTextEdit::ensureCursorVisible()
{
	TextBlock fvBlock = firstVisibleBlock();
	TextBlock curBlock = m_textCursor->block();
	if( curBlock.blockNumber() < fvBlock.blockNumber() ) {
		verticalScrollBar()->setValue(/*fm.lineSpacing() **/ curBlock.blockNumber());
		viewport()->update();
		return;
	}
	QRect vr = viewport()->rect();
	QFontMetrics fm = fontMetrics();
	const int nLines = vr.height() / fm.lineSpacing();
	const int t = curBlock.blockNumber() - fvBlock.blockNumber() - nLines;
	if( t < 0 ) return;		//	画面内
	index_t bn;
	if( t < 4 )
		bn = fvBlock.blockNumber() + t + 1;
	else
		bn = curBlock.blockNumber();
	verticalScrollBar()->setValue(/*fm.lineSpacing() **/ bn);
	viewport()->update();
}

bool PlainTextEdit::event ( QEvent * event )
{
	if( event->type() == QEvent::KeyPress ) {
		QKeyEvent *k = static_cast<QKeyEvent *>(event);
		if( k->key() == Qt::Key_Tab /*|| k->key() == Qt::Key_Backtab*/ ) {
			m_textCursor->insertText(QString("\t"));
			viewport()->update();
			return true;
		}
	}
	return QAbstractScrollArea::event(event);
}
void PlainTextEdit::keyPressEvent ( QKeyEvent * keyEvent )
{
	Qt::KeyboardModifiers mod = keyEvent->modifiers();
	const bool ctrl = (mod & Qt::ControlModifier) != 0;
	const bool shift = (mod & Qt::ShiftModifier) != 0;
	const uchar mvMode = shift ? TextCursor::KeepAnchor : TextCursor::MoveAnchor;
	uchar move = 0;
	switch( keyEvent->key() ) {
	case Qt::Key_Home:
		if( ctrl )
			move = TextCursor::StartOfDocument;
		else
			move = TextCursor::StartOfBlock;
		break;;
	case Qt::Key_End:
		if( ctrl )
			move = TextCursor::EndOfDocument;
		else
			move = TextCursor::EndOfBlock;
		break;
	case Qt::Key_Right:
		move = TextCursor::Right;
		break;
	case Qt::Key_Left:
		move = TextCursor::Left;
		break;
	case Qt::Key_Up:
		move = TextCursor::Up;
		break;
	case Qt::Key_Down:
		move = TextCursor::Down;
		break;
	case Qt::Key_Backspace:
		m_textCursor->deletePreviousChar();
		ensureCursorVisible();
		viewport()->update();
		return;
	case Qt::Key_Delete:
		m_textCursor->deleteChar();
		viewport()->update();
		return;
	case Qt::Key_Escape:
		m_textCursor->clearSelection();
		viewport()->update();
		emit showMessage( QString("%1 cur=(%2 %3 %4)")
							.arg(QDir::currentPath())
							.arg(m_textCursor->position())
							.arg(m_textCursor->blockData().index())
							.arg(m_textCursor->blockData().position()) );
		return;
	}
	if( move != 0 ) {
		m_textCursor->movePosition(move, mvMode);
		ensureCursorVisible();
		viewport()->update();
		return;
	}
	QString text = keyEvent->text();
	if( !text.isEmpty() ) {
		m_textCursor->insertText(text);
		viewport()->update();
	}
}
void PlainTextEdit::wheelEvent ( QWheelEvent * event )
{
	Qt::KeyboardModifiers mod = event->modifiers ();
	if( (mod & Qt::ControlModifier) != 0 ) {
		makeFontBigger(event->delta() > 0);
	} else
		QAbstractScrollArea::wheelEvent(event);
}
void PlainTextEdit::makeFontBigger(bool bigger)
{
	int sz = font().pointSize();
	if( bigger )
		++sz;
	else if( !--sz ) return;
	setFontPointSize(sz);
}
void PlainTextEdit::setFontPointSize(int sz)
{
	QFont ft = font();
	ft.setPointSize(sz);
	setFont(ft);
	onFontChanged();
	//emit showMessage(QString(tr("fontSize:%1").arg(sz)));
}
void PlainTextEdit::setFontFamily(const QString &name)
{
	QFont ft = font();
	ft.setFamily(name);
	setFont(ft);
	onFontChanged();
	//emit showMessage(QString(tr("fontSize:%1").arg(sz)));
}
void PlainTextEdit::copy()
{
	if( !m_textCursor->hasSelection() ) return;
	const QString text = m_textCursor->selectedText();
	if( text.isEmpty() ) return;
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text);
}
void PlainTextEdit::cut()
{
	if( !m_textCursor->hasSelection() ) return;
	copy();
	m_textCursor->deleteChar();
	viewport()->update();
}
void PlainTextEdit::paste()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();
	if( !text.isEmpty() ) {
		m_textCursor->insertText(text);
		viewport()->update();
	}
}
void PlainTextEdit::undo()
{
	index_t pos = 0;
	m_document->doUndo(pos);
	m_textCursor->setPosition(pos);
	viewport()->update();
}
void PlainTextEdit::redo()
{
	index_t pos = 0;
	m_document->doRedo(pos);
	m_textCursor->setPosition(pos);
	viewport()->update();
}
void PlainTextEdit::resizeEvent(QResizeEvent *event)
{
	QAbstractScrollArea::resizeEvent(event);
	updateLineNumberAreaSize();
	onBlockCountChanged();
}
void PlainTextEdit::updateLineNumberAreaSize()
{
	//QRect r = contentsRect();
	QRect r = rect();
	m_lineNumberArea->setGeometry(QRect(r.left(), r.top(), m_lineNumberAreaWidth, r.height()));
}
bool PlainTextEdit::eventFilter(QObject *obj, QEvent *event)
{
	if( obj == m_lineNumberArea && event->type() == QEvent::Paint ) {
		drawLineNumbers();
		return true;
	}
	return false;
}
void PlainTextEdit::drawLineNumbers()
{
	//qDebug() << "drawLineNumbers()";
	QPainter painter(m_lineNumberArea);
	painter.setPen(Qt::black);
	QRect ar = m_lineNumberArea->rect();
	painter.fillRect(ar, Qt::lightGray);
	//const int ht = fontMetrics().height();
	//QTextCursor cur = textCursor();
	TextBlock block = firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
	QFontMetrics fm = fontMetrics();
    int y = 0;
	while( block.isValid() && y < ar.height() ) {
		QString number = QString::number(lineNumber);
		painter.drawText(0, y, m_lineNumberWidth, fm.height(), Qt::AlignRight, number);
#if 0
		cur.setPosition(block.position());
		QRect r = cursorRect(cur);
		int y = r.bottom();
	    //const int y = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	    //if( y >= ar.bottom() ) break;
		QString number = QString::number(lineNumber);
		painter.drawText(0, r.top(), m_lineNumberWidth, r.height(), Qt::AlignRight, number);
	    if( y >= ar.bottom() ) break;
#endif
		++lineNumber;
		block = block.next();
		y += fm.lineSpacing();
	}
}
void PlainTextEdit::doJump(int lineNum)
{
#if 0
	ViCursor cur = viCursor();
	if( cur.movePosition(ViMoveOperation::JumpLine, QTextCursor::MoveAnchor, lineNum) )
		setViCursor(cur);
#endif
}
