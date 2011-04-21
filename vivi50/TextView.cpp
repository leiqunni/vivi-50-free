//----------------------------------------------------------------------
//
//			File:			"TextView.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextView クラス実装
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
#include "TextView.h"
#include	"TextDocument.h"
#include	"textCursor.h"
#include	"FindDlg.h"
#include	"ReplaceDlg.h"
#include	<math.h>
#include	<QDebug>

size_t UTF8CharSize(uchar ch);

bool hasSelection(const std::vector<ViewTextCursor*> &v)
{
	for(std::vector<ViewTextCursor*>::const_iterator itr = v.begin(),
														iend = v.end();
		itr != iend; ++itr)
	{
		if( (*itr)->hasSelection() ) return true;
	}
	return false;
}
void print(const std::vector<ViewTextCursor*> &v)
{
#ifdef	_DEBUG
	for(std::vector<ViewTextCursor*>::const_iterator itr = v.begin(),
														iend = v.end();
		itr != iend; ++itr)
	{
		TextBlock block = (*itr)->block();
		qDebug() << "anc = " << (*itr)->anchor()
					<< ", pos = " << (*itr)->position()
					<< ", block = " << block.index() << " " << block.position();
	}
#endif
}
//----------------------------------------------------------------------

#define		MARGIN_LEFT		4
//#define		LINE_HEIGHT		20

TextView::TextView(QWidget *parent)
	: QAbstractScrollArea(parent)
{
	m_mouseCaptured = false;
	m_toDeleteIMEPreeditText = false;
	m_drawCursor = true;
	m_wordWrapLongLines = false;
	m_firstViewLine = 0;
	m_lastViewLine = 0;
	//m_lineNumberWidth = 6;
	viewport()->setCursor(Qt::IBeamCursor);

	m_blocks.push_back(ViewTextBlockItem(0));
	m_document = new TextDocument();
	m_textCursor = new ViewTextCursor(this);
	m_preeditPosCursor = new ViewTextCursor(this);
	connect(m_document, SIGNAL(blockCountChanged()), this, SLOT(onBlockCountChanged()));

	m_lineNumberArea = new QWidget(this);
	m_lineNumberArea->installEventFilter(this);
	setFontPointSize(11);
	setAttribute(Qt::WA_InputMethodEnabled);
	//onFontChanged();
	m_viewportWidth = viewport()->width();

	//m_document->setPlainText(QString("LINE-1\nLINE-2\nLINE-3\n"));
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_timer->start(800);		//	
#if 0
	m_timer = new QElapsedTimer;
	m_timer->start();
	resetCursorBlinkTimer();
#endif
}

TextView::~TextView()
{
	delete m_document;
	delete m_textCursor;
}
size_t TextView::size() const
{
	return document()->size();
}
size_t TextView::lineCount() const
{
	return document()->blockCount() - (m_lastViewLine - m_firstViewLine)
			+ m_viewLines.size();
}
TextBlockData TextView::findBlockData(index_t position) const
{
	if( m_blocks.size() == 1 )
		return TextBlockData(0, 0);
	TextBlockData data(0, 0), next;
	if( m_blockData.m_index == 0 ) {		//	キャッシュが無い場合
		if( position <= size() / 2 ) {
			while( data.m_index < m_blocks.size() - 1 &&
					position >= (next = nextBlockData(data)).position() )
				data = next;
		} else {
			data = TextBlockData(m_blocks.size(), size());
			do {
				data = prevBlockData(data);
			} while( data.position() > position );
		}
	} else {
		if( position < m_blockData.position() ) {
			if( position <= m_blockData.position() / 2 ) {
				while( data.m_index < m_blockData.index() - 1 &&
						position >= (next = nextBlockData(data)).position() )
					data = next;
			} else {
				data = m_blockData;
				do {
					data = prevBlockData(data);
				} while( data.position() > position );
			}
		} else {
			next = nextBlockData(m_blockData);
			if( m_blockData.position() <= position && position < next.position() )
				return m_blockData;
			if( position <= m_blockData.position() + (size() - m_blockData.position()) / 2 ) {
				while( data.m_index < m_blocks.size() - 1 &&
						position >= (next = nextBlockData(data)).position() )
					data = next;
			} else {
				data = TextBlockData(m_blocks.size(), size());
				do {
					data = prevBlockData(data);
				} while( data.position() > position );
			}
		}
	}
	return data;
}
#if 0
void TextView::resetCursorBlinkTimer()
{
	m_tickCount = m_timer->elapsed();
}
#endif
void TextView::onTimer()
{
	m_drawCursor = !m_drawCursor;
	viewport()->update();
}
uchar TextView::charEncoding() const
{
	return document()->charEncoding();
}
bool TextView::withBOM() const
{
	return document()->withBOM();
}
QString TextView::toPlainText() const
{
	return document()->toPlainText();
}
bool TextView::isModified() const
{
	return document()->isModified();
}

int TextView::lineNumberLength() const
{
	const size_t bc = document()->blockCount();
	if( bc < 10000 ) return 6;
	return (int)log10( (double)bc ) + 2;
}

/*
		垂直スクロールバー情報を更新する必要があるのは、以下の３つの場合
			[1] ブロックカウントが変化した場合
			[2] フォントサイズが変化した場合
			[3] ウィンドウサイズが変化した場合
*/
void TextView::onFontChanged()
{
	//setTabStopWidth(fontMetrics().width('>') * 4);		//	tab 4
	const int len = lineNumberLength();
	m_lineNumberWidth = fontMetrics().width('8') * len;
	m_lineNumberAreaWidth = fontMetrics().width('8') * (len + 2);
	setViewportMargins(m_lineNumberAreaWidth, 0, 0, 0);
	updateLineNumberAreaSize();
	updateScrollBarData();
}
void TextView::onBlockCountChanged()
{
	//updateScrollBarData();
	onFontChanged();
}

void TextView::updateScrollBarData()
{
	QFontMetrics fm = fontMetrics();
	QSize areaSize = viewport()->size();
	//QSize  widgetSize = widget->size();

	verticalScrollBar()->setPageStep(areaSize.height() / fm.lineSpacing());
	verticalScrollBar()->setSingleStep(1);
	verticalScrollBar()->setRange(0, document()->blockCount() - areaSize.height() / fm.lineSpacing());
	//horizontalScrollBar()->setPageStep(widgetSize.width());
	//horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
	//updateWidgetPosition();

	m_lineNumberArea->update();
}
void TextView::focusInEvent ( QFocusEvent * event )
{
	QAbstractScrollArea::focusInEvent( event );
	const QString fullPath = document()->fullPath();
	if( !fullPath.isEmpty() ) {
		QDir dir(fullPath);
		dir.cdUp();
		QDir::setCurrent(dir.path());
	}
}

int TextView::xToOffset(const QString &text, int x) const
{
	const QFontMetrics fm = fontMetrics();
	const int spaceWidth = fm.width(QChar(' '));
	const int tabWidth = spaceWidth * 4;		//	とりあえず空白4文字分に固定
	const int limit = qMin(spaceWidth/8, 4);
	int width = 0;
	int ix = 0;
	while( ix < text.length() ) {
		QChar qch = text[ix];
		if( qch == '\r' || qch == '\n' ) break;
		if( qch == '\t' ) {
			width = (width / tabWidth + 1) * tabWidth;
		} else {
			width += fm.width(qch);
		}
		if( x <= width - limit )	//	
			break;
		++ix;
	}
	return ix;
}
int TextView::offsetToX(const QString &text, int offset) const
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
#if 0
			x += fm.width(text[ix++]);
#else
			int first = ix;
			while( ix < offset && text[ix] != '\t' && text[ix] != ' ' )
				++ix;
			const QString buf = text.mid(first, ix - first);
			x += fm.width(buf);
#endif
		}
	}
	return x;
}

TextBlock TextView::yToTextBlock(int py) const
{
	QWidget *vp = viewport();
	QRect vr = vp->rect();
	QFontMetrics fm = fontMetrics();

	int y = 0;
	TextBlock block = m_document->findBlockByNumber(verticalScrollBar()->value() /*/ fm.lineSpacing()*/);
	while( y < vr.height() && block.isValid() ) {
		const int nextY = y + fm.lineSpacing();
		if( py < nextY )
			break;
		y = nextY;
		block = block.next();
	}
	return block;
}
TextBlock TextView::firstVisibleBlock() const
{
	//QFontMetrics fm = fontMetrics();
	return m_document->findBlockByNumber(verticalScrollBar()->value());
}
int TextView::textBlockToY(const TextBlock &block) const
{
	TextBlock fvBlock = firstVisibleBlock();
	if( block < fvBlock ) return -1;
	QWidget *vp = viewport();
	QRect vr = vp->rect();
	int y = (block.index() - fvBlock.index()) * fontMetrics().lineSpacing();
	if( y >= vr.height() )
		return -1;
	return y;
}
int getEOLOffset(const QString text);
void TextView::paintEvent(QPaintEvent * event)
{
	//qDebug() << "blockData.index = " << m_document->blockData().index();
	//qDebug() << verticalScrollBar()->value();

	QWidget *vp = viewport();
	QRect vr = vp->rect();
	QPainter painter(vp);

	QFontMetrics fm = fontMetrics();
	const int spaceWidth = fm.width(QChar(' '));
	const int tabWidth = spaceWidth * 4;		//	とりあえず空白4文字分に固定

	const index_t lastBlockNumber = m_document->lastBlock().blockNumber();
	int y = 0;
	TextBlock block = firstVisibleBlock();
		//m_document->findBlockByNumber(verticalScrollBar()->value() /*/ fm.lineSpacing()*/);

	//	マルチカーソル選択状態表示
	for(std::vector<ViewTextCursor>::const_iterator itr = m_multiCursor.begin(),
													iend = m_multiCursor.end();
		itr != iend; ++itr)
	{
		if( !itr->hasSelection() ) continue;		//	非選択状態の場合
		index_t selLast = itr->lastPosition();
		if( selLast <= block.position() ) continue;		//	選択箇所が最初のブロック以前の場合
		index_t selFirst = itr->firstPosition();
		while( y < vr.height() && block.isValid() &&
			selFirst >= block.position() + m_document->blockSize(block.index()) )
		{
			y += fm.lineSpacing();
			block = block.next();
		}
		for(;;) {
			if( y >= vr.height() || !block.isValid() ) break;
			index_t nextBlockPosition = block.position() + m_document->blockSize(block.index());
			if( selFirst < nextBlockPosition && selLast > block.position() ) {
				//	block が選択範囲内にある場合
				const QString text = block.text();
				int x1 = offsetToX(text, block.charsCount(qMax(block.position(), selFirst)));
				int x2 = offsetToX(text, block.charsCount(qMin(nextBlockPosition, selLast)));
				painter.fillRect(QRect(x1 + MARGIN_LEFT + 1, y+2, x2 - x1, fm.height()), Qt::lightGray);
			}
			if( selLast < nextBlockPosition ) break;
			y += fm.lineSpacing();
			block = block.next();
		}
		if( y >= vr.height() || !block.isValid() ) break;
	}

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
	y = 0;
	block = firstVisibleBlock();
		//m_document->findBlockByNumber(verticalScrollBar()->value() /*/ fm.lineSpacing()*/);
	//qDebug() << "firstVisibleBlock.index = " << block.index();
	//TextBlock block = m_document->firstBlock();
	std::vector<ViewTextCursor>::const_iterator mcitr = m_multiCursor.begin();
	std::vector<ViewTextCursor>::const_iterator mciend = m_multiCursor.end();
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
		if( m_drawCursor ) {
			if( m_textCursor->block() == block) {		//	カーソルがブロック内にある場合
				const int offset = qMin(block.charsCount(m_textCursor->position()),
											text.length());
				int x = offsetToX(text, offset);
				painter.fillRect(QRect(x + MARGIN_LEFT + 1, y+2, 2, fm.height()), Qt::red);
			}
			while( mcitr != mciend && mcitr->block() < block )
				++mcitr;
			while( mcitr != mciend && mcitr->block() == block) {		//	カーソルがブロック内にある場合
				const int offset = qMin(block.charsCount(mcitr->position()),
											text.length());
				int x = offsetToX(text, offset);
				painter.fillRect(QRect(x + MARGIN_LEFT + 1, y+2, 2, fm.height()), Qt::green);
				++mcitr;
			}
		}
		//painter.drawText(MARGIN_LEFT, y + fm.ascent(), text);
		int x = 0;
		int ix = 0;
		int EOLOffset = getEOLOffset(text);
		while( ix < EOLOffset ) {
			if( text[ix] == ' ' ) {
				x += spaceWidth;
				++ix;
			} else if( text[ix] == '\t' ) {
				painter.setPen(Qt::lightGray);
				painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), ">");
				++ix;
				x = (x / tabWidth + 1) * tabWidth;
			} else {
				int first = ix;
				while( ix < EOLOffset && text[ix] != ' ' && text[ix] != '\t' )
					++ix;
				const QString buf = text.mid(first, ix - first);
				painter.setPen(Qt::black);
				painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), buf);
				x += fm.width(buf);
			}
		}
		if( ix < text.length() ) {
			painter.setPen(Qt::lightGray);
			QString nl;
			if( text[ix] == '\n' )
				nl = QChar(0x266a);		//	♪
			else if( ix + 1 < text.length() )
				nl = QChar(0x266c);
			else
				nl = QChar(0x2669);
			painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), nl);
		}
		if( block.blockNumber() == lastBlockNumber ) {
			painter.setPen(Qt::blue);
			const int x = offsetToX(text, text.length());
			painter.drawText(x + MARGIN_LEFT, y + fm.ascent(), "[EOF]");
			break;
		}
		if( !m_preeditString.isEmpty() && block.index() == m_preeditPosCursor->block().index() ) {
			painter.setPen(Qt::blue);
			int x1 = offsetToX(text, block.charsCount(m_preeditPosCursor->anchor())) + MARGIN_LEFT;
			int x2 = offsetToX(text, block.charsCount(m_preeditPosCursor->position())) + MARGIN_LEFT;
			const int uy = y + fm.ascent() + 1;
			painter.drawLine(x1, uy, x2, uy);
		}
		block = block.next();
		y += fm.lineSpacing();
	}
	//qDebug() << m_preeditPosCursor->block().index() << " '" << m_preeditString << "'";
	m_lineNumberArea->update();
	//qDebug() << "blockData.index = " << m_document->blockData().index();
}
void TextView::ensureCursorVisible()
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
		bn = qMin( curBlock.blockNumber(), (size_t)verticalScrollBar()->maximum() );
	verticalScrollBar()->setValue(/*fm.lineSpacing() **/ bn);
	viewport()->update();
}
void TextView::removeOverlappedCursor()
{
	for(std::vector<ViewTextCursor>::iterator itr = m_multiCursor.begin(),
												iend = m_multiCursor.end();
		itr != iend; ++itr)
	{
		if( itr->isOverlapped(*m_textCursor) ) {
			m_multiCursor.erase(itr);
			break;
		}
	}
}

bool TextView::event ( QEvent * event )
{
	if( event->type() == QEvent::KeyPress ) {
		QKeyEvent *k = static_cast<QKeyEvent *>(event);
		if( k->key() == Qt::Key_Tab || k->key() == Qt::Key_Backtab ) {
			insertText(QString("\t"), true);
			ensureCursorVisible();
			viewport()->update();
			return true;
		}
#if 0	//	うまくいかない 11/04/13
		if( k->key() == Qt::Key_Delete ) {
			keyPressEvent(k);
			return true;
		}
#endif
	}
#if 0
	if( event->type() == QEvent::InputMethod ) {
		QInputMethodEvent *k = static_cast<QInputMethodEvent *>(event);
		inputMethodEvent(k);
		return true;
	}
#endif
	return QAbstractScrollArea::event(event);
}
QVariant TextView::inputMethodQuery ( Qt::InputMethodQuery query ) const
{
	if( query == Qt::ImMicroFocus ) {
		TextBlock block = m_textCursor->block();
		int x = offsetToX(block.text(), block.charsCount(m_preeditPosCursor->anchor()));
		int y = textBlockToY(block);
		return QVariant(QRect(m_lineNumberAreaWidth + x, y, 20, 20));
	}
	return QAbstractScrollArea::inputMethodQuery(query);
}
void TextView::inputMethodEvent ( QInputMethodEvent * event )
{
	//qDebug() << "*** inputMethodEvent " << event;
	if( m_toDeleteIMEPreeditText ) {
		//qDebug() << "  doUndo.";
		//if( event->preeditString() == QString("かｎ") )
		//	qDebug() << "  かｎ";
		m_preeditString = QString();
		undo();
#if 0
		index_t position;
		m_document->doUndo(position);
		m_textCursor->setPosition(pos);
		ensureCursorVisible();
		viewport()->update();
#endif
		m_toDeleteIMEPreeditText = false;
	}
	const QString &text = event->commitString();
	if( !text.isEmpty() ) {
		//qDebug() << "  insert commitString " << text;
		insertText(text);
		viewport()->update();
	}
	m_preeditString = event->preeditString();
	if( !m_preeditString.isEmpty() ) {
		//qDebug() << "  start = " << event->replacementStart () <<
		//			", len = " << event->replacementLength ();
		//qDebug() << "  insertText " << peText;
		m_preeditPosCursor->setAnchor(m_textCursor->position());
		insertText(*m_textCursor, m_preeditString);
		m_preeditPosCursor->setPosition(m_textCursor->position(), TextCursor::KeepAnchor);
		m_toDeleteIMEPreeditText = true;
		viewport()->update();
	}
	QAbstractScrollArea::inputMethodEvent( event );
}
void TextView::keyPressEvent ( QKeyEvent * keyEvent )
{
	Qt::KeyboardModifiers mod = keyEvent->modifiers();
	const bool ctrl = (mod & Qt::ControlModifier) != 0;
	const bool shift = (mod & Qt::ShiftModifier) != 0;
	const uchar mvMode = shift ? TextCursor::KeepAnchor : TextCursor::MoveAnchor;
	uchar move = 0;
	uint repCount = 1;
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
		if( ctrl )
			move = TextCursor::NextWord;
		else
			move = TextCursor::Right;
		break;
	case Qt::Key_Left:
		if( ctrl )
			move = TextCursor::PrevWord;
		else
			move = TextCursor::Left;
		break;
	case Qt::Key_Up:
		move = TextCursor::Up;
		break;
	case Qt::Key_Down:
		move = TextCursor::Down;
		break;
	case Qt::Key_PageUp:
		repCount = verticalScrollBar()->pageStep();
		verticalScrollBar()->setValue( qMax(0,
								(int)(verticalScrollBar()->value() - repCount)));
		move = TextCursor::Up;
		break;
	case Qt::Key_PageDown:
		repCount = verticalScrollBar()->pageStep();
		verticalScrollBar()->setValue( verticalScrollBar()->value() + repCount);
		move = TextCursor::Down;
		break;
	case Qt::Key_Backspace:
		if( !ctrl && !shift )
			deletePreviousChar();
		else {
			if( !m_textCursor->hasSelection() ) {
				if( ctrl ) {
					m_textCursor->movePosition(TextCursor::PrevWord, TextCursor::KeepAnchor);
					if( !m_textCursor->hasSelection() )
						return;
				} else if( shift ) {
					m_textCursor->movePosition(TextCursor::StartOfBlock, TextCursor::KeepAnchor);
					if( !m_textCursor->hasSelection() )
						return;
				}
			}
			m_textCursor->deletePreviousChar();
		}
		ensureCursorVisible();
		viewport()->update();
		return;
	case Qt::Key_Delete:
		if( !ctrl && !shift )
			deleteChar();
		else {
			if( !m_textCursor->hasSelection() ) {
				if( ctrl ) {
					m_textCursor->movePosition(TextCursor::NextWord, TextCursor::KeepAnchor);
					if( !m_textCursor->hasSelection() )
						return;
				} else if( shift ) {
					m_textCursor->movePosition(TextCursor::EndOfBlock, TextCursor::KeepAnchor);
					if( !m_textCursor->hasSelection() )
						return;
				}
			}
			m_textCursor->deleteChar();
		}
		viewport()->update();
		return;
	case Qt::Key_Return:
		insertText(QString("\n"));	//	undone C 本当は設定で CRLF/CR/LF のいずれかを挿入
		ensureCursorVisible();
		viewport()->update();
		return;
	case Qt::Key_Escape:
		m_textCursor->clearSelection();
		clearMultiCursor();
		viewport()->update();
		emit showMessage( QString("%1 cur=(%2 %3 %4) blockData=(%5 %6)")
							.arg(QDir::currentPath())
							.arg(m_textCursor->position())
							.arg(m_textCursor->blockData().index())
							.arg(m_textCursor->blockData().position())
							.arg(m_document->blockData().index())
							.arg(m_document->blockData().position()) );
		return;
	}
	if( move != 0 ) {
		m_textCursor->movePosition(move, mvMode, repCount);
		removeOverlappedCursor();
		ensureCursorVisible();
		m_drawCursor = true;
		m_timer->start();		//	タイマーリスタート
		viewport()->update();
		return;
	}
	QString text = keyEvent->text();
	if( !text.isEmpty() ) {
		insertText(text);
		ensureCursorVisible();
		viewport()->update();
	}
}
void TextView::wheelEvent ( QWheelEvent * event )
{
	Qt::KeyboardModifiers mod = event->modifiers ();
	if( (mod & Qt::ControlModifier) != 0 ) {
		makeFontBigger(event->delta() > 0);
	} else
		QAbstractScrollArea::wheelEvent(event);
}
void TextView::makeFontBigger(bool bigger)
{
	int sz = font().pointSize();
	if( bigger )
		++sz;
	else if( !--sz ) return;
	setFontPointSize(sz);
	emit showMessage(QString(tr("fontPointSize:%1").arg(sz)));
}
void TextView::setFontPointSize(int sz)
{
	QFont ft = font();
	ft.setPointSize(sz);
	setFont(ft);
	onFontChanged();
	//emit showMessage(QString(tr("fontSize:%1").arg(sz)));
}
void TextView::setFontFamily(const QString &name)
{
	QFont ft = font();
	ft.setFamily(name);
	setFont(ft);
	onFontChanged();
	//emit showMessage(QString(tr("fontSize:%1").arg(sz)));
}
void TextView::selectAll()
{
	m_textCursor->movePosition(TextCursor::StartOfDocument);
	m_textCursor->movePosition(TextCursor::EndOfDocument, TextCursor::KeepAnchor);
	ensureCursorVisible();
	viewport()->update();
}
void TextView::copy()
{
	if( !m_textCursor->hasSelection() ) return;
	const QString text = m_textCursor->selectedText();
	if( text.isEmpty() ) return;
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text);
}
void TextView::cut()
{
	if( !m_textCursor->hasSelection() ) return;
	copy();
	if( !hasMultiCursor() ) {
		if( m_textCursor->hasSelection() )
			m_textCursor->deleteChar();
	} else {
		document()->openUndoBlock();
		std::vector<ViewTextCursor*> v;			//	メインカーソルも含めたカーソル一覧（昇順ソート済み）
		getAllCursor(v);
		for(std::vector<ViewTextCursor*>::iterator itr = v.begin(), iend = v.end();
			itr != iend; ++itr)
		{
			if( (*itr)->hasSelection() ) {
				const int sz = document()->deleteChar(**itr);
				for(std::vector<ViewTextCursor*>::iterator k = itr; ++k != iend; ) {
					(*k)->move(-sz);
				}
			}
		}
		document()->closeUndoBlock();
	}
	viewport()->update();
}
void TextView::paste()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();
	if( !text.isEmpty() ) {
		insertText(text);
		ensureCursorVisible();
		viewport()->update();
	}
}
void TextView::undo()
{
	if( !m_document->canUndo() ) return;
	clearMultiCursor();
	index_t pos = 0, anchor = 0;
	m_document->doUndo(pos, anchor);
	if( pos == anchor )
		m_textCursor->setPosition(pos);
	else {
		m_textCursor->setPosition(anchor);
		m_textCursor->setPosition(pos, TextCursor::KeepAnchor);
	}
	ensureCursorVisible();
	viewport()->update();
}
void TextView::redo()
{
	if( !m_document->canRedo() ) return;
	clearMultiCursor();
	index_t pos = 0, anchor = 0;
	m_document->doRedo(pos, anchor);
	m_textCursor->setPosition(pos);
	ensureCursorVisible();
	viewport()->update();
}
void TextView::replace()
{
	clearMultiCursor();
	ReplaceDlg aDlg;
	connect(&aDlg, SIGNAL(doFindNext(const QString &, ushort)),
			this, SLOT(doFindNext(const QString &, ushort)));
	connect(&aDlg, SIGNAL(isMatched(bool &, const QString &, ushort)),
			this, SLOT(isMatched(bool &, const QString &, ushort)));
	connect(&aDlg, SIGNAL(doReplace(const QString &)),
			this, SLOT(doReplace(const QString &)));
	connect(&aDlg, SIGNAL(doReplaceAll(const QString &, ushort, const QString &)),
			this, SLOT(doReplaceAll(const QString &, ushort, const QString &)));
	connect(&aDlg, SIGNAL(doUndo()), this, SLOT(undo()));
	connect(&aDlg, SIGNAL(doRedo()), this, SLOT(redo()));
	aDlg.exec();
}
void TextView::find()
{
	FindDlg aDlg;
	connect(&aDlg, SIGNAL(doFindNext(const QString &, ushort)),
			this, SLOT(doFindNext(const QString &, ushort)));
	aDlg.exec();
}
void TextView::doFindNext(const QString &text, ushort options)
{
	if( text.isEmpty() ) return;
	TextCursor c = document()->find(text, *m_textCursor, options);
	if( !c.isNull() ) {
		*m_textCursor = c;
		ensureCursorVisible();
		viewport()->update();
	}
}
void TextView::isMatched(bool &b, const QString &text, ushort options)
{
	b = false;
	if( !m_textCursor->hasSelection() || text.isEmpty() ) return;
	b = document()->isMatched(text, *m_textCursor, options);
}
void TextView::findNext()
{
	QString text;
	getLastFindString(text);
	if( text.isEmpty() ) return;
	ushort options = 0;
	doFindNext(text, options);
}
void TextView::findPrev()
{
	QString text;
	getLastFindString(text);
	if( text.isEmpty() ) return;
	ushort options = 0;
	doFindNext(text, options | FindBackWard);
}
void TextView::findCurWord()
{
	if( !m_textCursor->hasSelection() ) {
		m_textCursor->movePosition(TextCursor::StartOfWord);
		m_textCursor->movePosition(TextCursor::EndOfWord, TextCursor::KeepAnchor);
	}
	if( m_textCursor->hasSelection() ) {
		QString text = m_textCursor->selectedText();
		doFindNext(text, 0);
		addFindStringHist(text);
	}
}
void TextView::doReplace(const QString &text)
{
	if( m_textCursor->hasSelection() )
		insertText(text);
}
void TextView::doReplaceAll(const QString &findText, ushort options,
							const QString &replaceText)
{
	document()->doReplaceAll(findText, options, replaceText);
	ensureCursorVisible();
	viewport()->update();
}
void TextView::resizeEvent(QResizeEvent *event)
{
	QAbstractScrollArea::resizeEvent(event);
	updateLineNumberAreaSize();
	//onBlockCountChanged();
	updateScrollBarData();
}
void TextView::updateLineNumberAreaSize()
{
	//QRect r = contentsRect();
	size_t bc = m_document->blockCount();
	QRect r = rect();
	m_lineNumberArea->setGeometry(QRect(r.left(), r.top(), m_lineNumberAreaWidth, r.height()));
}
bool TextView::eventFilter(QObject *obj, QEvent *event)
{
	if( obj == m_lineNumberArea && event->type() == QEvent::Paint ) {
		drawLineNumbers();
		return true;
	}
	return false;
}
void TextView::drawLineNumbers()
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
void TextView::doJump(int lineNum)
{
	ViewTextCursor cur(this);
	if( cur.movePosition(TextCursor::Down, TextCursor::MoveAnchor, lineNum - 1) ) {
		*m_textCursor = cur;
		ensureCursorVisible();
		viewport()->update();
	}
#if 0
	ViCursor cur = viCursor();
	if( cur.movePosition(ViMoveOperation::JumpLine, QTextCursor::MoveAnchor, lineNum) )
		setViCursor(cur);
#endif
}
void TextView::mousePressEvent ( QMouseEvent * event )
{
	Qt::KeyboardModifiers mod = event->modifiers();
	const bool ctrl = (mod & Qt::ControlModifier) != 0;
	const bool shift = (mod & Qt::ShiftModifier) != 0;
	TextBlock block = yToTextBlock(event->y());
	if( !block.isValid() )
		block = document()->lastBlock();
	//qDebug() << "block index = " << block.index();
	int offset = xToOffset(block.text(), event->x());
	//qDebug() << "offset = " << offset;
	ViewTextCursor cur(*m_textCursor);
	cur.setPosition(block.position());
	if( offset != 0 )
		cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, offset);
	if( ctrl ) {
		std::vector<ViewTextCursor>::iterator itr = m_multiCursor.begin();
		std::vector<ViewTextCursor>::iterator iend = m_multiCursor.end();
		while( itr != iend && itr->position() < m_textCursor->position() )
			++itr;
		m_multiCursor.insert(itr, *m_textCursor);
	} else
		clearMultiCursor();
	*m_textCursor = cur;
	removeOverlappedCursor();
	m_timer->start();		//	タイマーリスタート
	viewport()->update();
	m_mouseCaptured = true;
}
void TextView::mouseReleaseEvent ( QMouseEvent * event )
{
	m_mouseCaptured = false;
}
void TextView::mouseMoveEvent ( QMouseEvent * event )
{
	if( m_mouseCaptured ) {
		TextBlock block = yToTextBlock(event->y());
		if( !block.isValid() )
			block = document()->lastBlock();
		int offset = xToOffset(block.text(), event->x());
		m_textCursor->setPosition(block.position(), TextCursor::KeepAnchor);
		if( offset != 0 )
			m_textCursor->movePosition(TextCursor::Right, TextCursor::KeepAnchor, offset);
		removeOverlappedCursor();
		viewport()->update();
	}
}
void TextView::mouseDoubleClickEvent ( QMouseEvent * event )
{
	m_textCursor->movePosition(TextCursor::StartOfWord);
	m_textCursor->movePosition(TextCursor::EndOfWord, TextCursor::KeepAnchor);
	removeOverlappedCursor();
	viewport()->update();
}
void TextView::getAllCursor(std::vector<ViewTextCursor*> &v)
{
	v.clear();
	v.reserve(m_multiCursor.size() + 1);
	bool inserted = false;
	for(std::vector<ViewTextCursor>::iterator itr = m_multiCursor.begin(),
														iend = m_multiCursor.end();
		itr != iend; ++itr)
	{
		if( !inserted && m_textCursor->position() <= itr->position() ) {
			v.push_back(m_textCursor);
			inserted = true;
		}
		v.push_back(&*itr);
	}
	if( !inserted )
		v.push_back(m_textCursor);
}
void TextView::deleteChar()
{
	if( !hasMultiCursor() )
		document()->deleteChar(*m_textCursor);
	else {
		//	undone R insertText と処理を共通化
		document()->openUndoBlock();
		std::vector<ViewTextCursor*> v;			//	メインカーソルも含めたカーソル一覧（昇順ソート済み）
		getAllCursor(v);
		for(std::vector<ViewTextCursor*>::iterator itr = v.begin(), iend = v.end();
			itr != iend; ++itr)
		{
			const int sz = document()->deleteChar(**itr);
			for(std::vector<ViewTextCursor*>::iterator k = itr; ++k != iend; ) {
				(*k)->move(-sz);
				//(*k)->setPosition((*k)->position() - sz);
				//(*k)->setAnchor((*k)->anchor() - sz);
			}
		}
		document()->closeUndoBlock();
	}
}
void TextView::deletePreviousChar()
{
	if( !hasMultiCursor() )
		document()->deletePreviousChar(*m_textCursor);
	else {
		//	undone R insertText と処理を共通化
		document()->openUndoBlock();
		std::vector<ViewTextCursor*> v;			//	メインカーソルも含めたカーソル一覧（昇順ソート済み）
		getAllCursor(v);
		for(std::vector<ViewTextCursor*>::iterator itr = v.begin(), iend = v.end();
			itr != iend; ++itr)
		{
			const int sz = document()->deletePreviousChar(**itr);
			for(std::vector<ViewTextCursor*>::iterator k = itr; ++k != iend; ) {
				(*k)->move(-sz);
				//(*k)->setPosition((*k)->position() - sz);
				//(*k)->setAnchor((*k)->anchor() - sz);
			}
		}
		document()->closeUndoBlock();
	}
}
void TextView::insertText(const QString &text, bool tab)
{
	if( !hasMultiCursor() )
		insertText(*m_textCursor, text);
	else {
		document()->openUndoBlock();
		std::vector<ViewTextCursor*> v;			//	メインカーソルも含めたカーソル一覧（昇順ソート済み）
		getAllCursor(v);
		if( tab && hasSelection(v) ) {
			//	選択領域がある場合はローテイト
			//print(v);
			std::vector<ViewTextCursor*>::iterator itr = v.end() - 1;
			const QString lastText = (*itr)->selectedText();
			int lastLength = lastText.length();
			for(;;) {
				const QString text = itr == v.begin() ? lastText
											: (*(itr-1))->selectedText();
				const int d = text.length() - lastLength;		//	増減値
				document()->insertText(**itr, text, /*select::=*/true);
				//print(v);
				for(std::vector<ViewTextCursor*>::iterator k = itr + 1; k != v.end(); ++k) {
					//	undone A BlockData も要補正
					(*k)->move(d);
					//(*k)->setAnchor((*k)->anchor() + d);
					//(*k)->setPosition((*k)->position() + d, TextCursor::KeepAnchor);
#if 0
					if( (*k)->anchor() < (*k)->position() )
						(*k)->setPosition((*k)->position() + d);
					else if( (*k)->position() < (*k)->anchor() )
						(*k)->setAnchor((*k)->anchor() + d);
#endif
				}
				//print(v);
				if( itr == v.begin() ) break;
				lastLength = text.length();
				--itr;
			}
		} else
			for(std::vector<ViewTextCursor*>::iterator itr = v.begin(), iend = v.end();
				itr != iend; ++itr)
			{
				const int sz = insertText(**itr, text);
				//print(v);
				for(std::vector<ViewTextCursor*>::iterator k = itr; ++k != iend; ) {
					(*k)->move(sz);
					//(*k)->setAnchor((*k)->anchor() + sz);
					//(*k)->setPosition((*k)->position() + sz, TextCursor::KeepAnchor);
				}
				//print(v);
			}
		document()->closeUndoBlock();
	}
}
int TextView::insertText(ViewTextCursor &cur, const QString &text)
{
	const size_t sz = document()->insertText(cur, text);
	//	undone B ブロック情報更新
	return sz;
}
void TextView::deleteChar(ViewTextCursor &cur)
{
	document()->deleteChar(cur);
	//	undone B ブロック情報更新
}
void TextView::deletePreviousChar(ViewTextCursor &cur)
{
	document()->deletePreviousChar(cur);
	//	undone B ブロック情報更新
}
void TextView::buildBlocks()
{
	QFontMetrics fm = fontMetrics();
	m_blocks.clear();
	//m_blocks.push_back(ViewTextBlockItem(0));
	TextBlock block = document()->firstBlock();
	while( block.isValid() ) {
		index_t pos = block.position();
		index_t blockPos = pos;
		QString text = block.text();
		const size_t nlLength = block.newlineLength();
		index_t ixEOL = text.length() - nlLength;		//	改行コードは１バイトと仮定
		if( !ixEOL )
			m_blocks.push_back(ViewTextBlockItem(block.size()));
		else {
			index_t ix = 0;
			while( ix < ixEOL ) {
				QString buf;
				for(;;) {
					if( ix == ixEOL ) {
						pos += nlLength;
						break;
					}
					buf += text.at(ix);
					if( fm.width(buf) > m_viewportWidth ) break;
					++ix;
					pos += UTF8CharSize((*document())[pos]);
				}
				m_blocks.push_back(ViewTextBlockItem(pos - blockPos));
				blockPos = pos;
			}
		}
		block = block.next();
	}
}
void TextView::onWordWrap(bool b)
{
	m_wordWrapLongLines = b;
	viewport()->update();
}
void TextView::buildLines(TextBlock block, int wd, int ht)
{
	QFontMetrics fm = fontMetrics();
	m_viewLines.clear();
	//m_blocks.push_back(ViewTextBlockItem(0));
	//TextBlock block = document()->firstBlock();
	index_t blockIndex = block.index();
	m_firstViewLine = blockIndex;
	while( block.isValid() ) {
		index_t pos = block.position();
		index_t blockPos = pos;
		QString text = block.text();
		const size_t nlLength = block.newlineLength();
		index_t ixEOL = text.length() - nlLength;		//	改行コードは１バイトと仮定
		if( !ixEOL )
			m_viewLines.push_back(ViewLine(pos, blockIndex));
		else {
			index_t ix = 0;
			while( ix < ixEOL ) {
				QString buf;
				for(;;) {
					if( ix == ixEOL ) {
						pos += nlLength;
						break;
					}
					buf += text.at(ix);
					if( fm.width(buf) > wd ) break;
					++ix;
					pos += UTF8CharSize((*document())[pos]);
				}
				m_viewLines.push_back(ViewLine(blockPos, blockIndex));
				blockPos = pos;
			}
		}
		++blockIndex;
		block = block.next();
	}
	m_lastViewLine = blockIndex;
}
