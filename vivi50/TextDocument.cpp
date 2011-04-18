//----------------------------------------------------------------------
//
//			File:			"TextDocument.cpp"
//			Created:		01-Apr-2011
//			Author:			Nobuhide Tsuda
//			Description:	TextDocument �N���X����
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

#include	<QtCore>
#include "TextDocument.h"
#include "textCursor.h"
#include	"FindDlg.h"		//	for �����I�v�V����
							//	undone R �����I�v�V���������̃t�@�C�������������������

static inline bool isUTF8FirstChar(uchar ch)
{
	return !(ch & 0x80) || (ch & 0x40) != 0;
}
//	UTF-8�f�[�^�̍ŏ��̃o�C�g�ɂ�蕶���o�C�g�����v�Z
size_t UTF8CharSize(uchar ch);
//----------------------------------------------------------------------
void GVUndoMgr::openUndoBlock()
{
	if( !m_undoBlockLevel )
		m_toSetBlockFlag = true;
	++m_undoBlockLevel;
}
void GVUndoMgr::closeUndoBlock()
{
	if( !m_undoBlockLevel ) return;
	if( !--m_undoBlockLevel && !m_toSetBlockFlag && !m_items.empty() )
		m_items[m_items.size() - 1].m_flags ^= GVUNDOITEM_BLOCK;
}
void GVUndoMgr::resetUndoBlock()
{
	if( !m_undoBlockLevel ) return;
	m_undoBlockLevel = 0;
	if( !m_toSetBlockFlag && !m_items.empty() )
		m_items[m_items.size() - 1].m_flags ^= GVUNDOITEM_BLOCK;
}
#if 1
void GVUndoMgr::push_back(const GVUndoItem &item, bool modified)
{
	if( m_current < m_items.size() ) {
		//	undone M �s�v�ȃq�[�v�̉��
		m_items.erase(m_items.begin() + m_current, m_items.end());
	}
	m_items.push_back(item);
	GVUndoItem *ptr = &m_items[m_items.size() - 1];
	if( !modified )
		ptr->m_flags |= GVUNDOITEM_UNDO_MF_OFF;
	if( m_toSetBlockFlag ) {
		ptr->m_flags |= GVUNDOITEM_BLOCK;
		m_toSetBlockFlag = false;
	}
	m_current = m_items.size();
}
#else
void GVUndoMgr::push_back(GVUndoItem *ptr, bool modified)
{
	if( m_current < m_items.size() ) {
		//	undone M �s�v�ȃq�[�v�̉��
		m_items.erase(m_items.begin() + m_current, m_items.end());
	}
	if( !modified )
		ptr->m_flags |= GVUNDOITEM_UNDO_MF_OFF;
//#if		!UNDOMGR_USE_OBJECT_POOL
//	m_items.push_back(boost::shared_ptr<GVUndoItem>(ptr));
//#else
	m_items.push_back(ptr);
//#endif
	m_current = m_items.size();
}
#endif

//	�h�L�������g���ۑ����ꂽ���ɃR�[�������
void GVUndoMgr::resetModifiedFlags()
{
	bool before = true;
	for(int ix = 0; ix < m_items.size(); ++ix) {
		if( ix == m_current ) before = false;
		GVUndoItem *undoItem = &m_items[ix];
		undoItem->m_beforeSave = before;
		undoItem->m_flags &= ~(GVUNDOITEM_UNDO_MF_OFF|GVUNDOITEM_REDO_MF_OFF);
		if( ix == m_current - 1 )
			undoItem->m_flags |= GVUNDOITEM_REDO_MF_OFF | GVUNDOITEM_SAVED;
		else if( ix == m_current)
			undoItem->m_flags |= GVUNDOITEM_UNDO_MF_OFF;
	}
}
//	@return		Undo �̌��ʁA���f�B�t�@�C�t���O���n�m�̏ꍇ�� TRUE ��Ԃ�
bool GVUndoMgr::doUndo(TextDocument *bb, uint& pos, uint& anchor)
{
	if( !m_current ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[--m_current];
	GVUndoItem *ptr;
	for(bool b = true;;) {
		ptr = &m_items[--m_current];
		switch( ptr->m_type ) {
		case GVUNDOITEM_TYPE_ERASE: {
			cuchar *heap = &m_heap[ptr->m_hp_ix];
			bb->insert(ptr->m_first, heap, heap + ptr->data_size());
			if( (ptr->m_flags & GVUNDOITEM_CUR_TAIL) != 0 )
				pos = (anchor = ptr->m_first) + ptr->data_size();
			else
				anchor = (pos = ptr->m_first) + ptr->data_size();
			break;
		}
		case GVUNDOITEM_TYPE_INSERT:
			if( ptr->m_rhp_ix == 0 )
				ptr->m_rhp_ix = addToRedoHeap(bb->begin() + ptr->m_first, bb->begin() + ptr->m_last);
			bb->erase(ptr->m_first, ptr->m_last);
			pos = anchor = ptr->m_first;
			break;
		case GVUNDOITEM_TYPE_REPLACE: {
			if( ptr->m_rhp_ix == 0 )
				ptr->m_rhp_ix = addToRedoHeap(bb->begin() + ptr->m_first, bb->begin() + ptr->m_last2);
			bb->erase(ptr->m_first, ptr->m_last2);
			cuchar *heap = &m_heap[ptr->m_hp_ix];
			bb->insert(ptr->m_first, heap, heap + ptr->data_size());
			if( (ptr->m_flags & GVUNDOITEM_CUR_TAIL) != 0 )
				pos = (anchor = ptr->m_first) + ptr->data_size();
			else
				anchor = (pos = ptr->m_first) + ptr->data_size();
			//pos = ptr->m_first + ptr->data_size();
			break;
		}
		}
		//	GVUNDOITEM_BLOCK �������Ă���ꍇ�� ���� GVUNDOITEM_BLOCK �������Ă�����̂܂ŏ�������
		const bool pb = (ptr->m_flags & GVUNDOITEM_BLOCK) != 0;
		if( !m_current || pb != b ) break;
		b = false;
	}
	return !(ptr->m_flags & GVUNDOITEM_UNDO_MF_OFF) ? true : false;
	//return true;
}
bool GVUndoMgr::doRedo(TextDocument *bb, uint& pos, uint& anchor)
{
	if( m_current >= m_items.size() ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[m_current];
	//++m_current;
	GVUndoItem *ptr;
	for(bool b = true;;) {
		ptr = &m_items[m_current++];
		switch( ptr->m_type ) {
		case GVUNDOITEM_TYPE_ERASE:
			bb->erase(ptr->m_first, ptr->m_last);
			pos = anchor = ptr->m_first;
			break;
		case GVUNDOITEM_TYPE_INSERT: {
			cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
			bb->insert(ptr->m_first, heap, heap + ptr->data_size());
			pos = anchor = ptr->m_first + ptr->data_size();
			break;
		}
		case GVUNDOITEM_TYPE_REPLACE: {
			bb->erase(ptr->m_first, ptr->m_last);
			cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
			bb->insert(ptr->m_first, heap, heap + ptr->data_size2());
			pos = (anchor = ptr->m_first) + ptr->data_size2();
			break;
		}
		}
		//	GVUNDOITEM_BLOCK �������Ă���ꍇ�� ���� GVUNDOITEM_BLOCK �������Ă�����̂܂ŏ�������
		const bool pb = (ptr->m_flags & GVUNDOITEM_BLOCK) != 0;
		if( m_current >= m_items.size() || pb != b ) break;
		b = false;
	}
	return !(ptr->m_flags & GVUNDOITEM_REDO_MF_OFF) ? true : false;
}
//----------------------------------------------------------------------
uint TextBlock::size() const
{
#if 1
	return m_document->blockSize(m_data.m_index);
#else
	if( !isValid() ) return 0;
	if( m_index == m_document->blockCount() - 1 )		//	�Ō�̃u���b�N
		return m_document->size() - m_document->blockPosition(m_index);
	else
		return m_document->blockPosition(m_index + 1) - m_document->blockPosition(m_index);
#endif
}
index_t TextBlock::position() const
{
	return isValid() ? m_data.m_position : 0;
}
uint TextBlock::newlineLength() const
{
	if( !isValid() ) return 0;
	const size_t sz = size();
	index_t np = position() + sz;
	if( np > position() ) {
		uchar uch = (*m_document)[np-1];
		if( uch == '\r' )
			return 1;
		if( uch == '\n' ) {
			if( np - 2 >= position() && (*m_document)[np-2] == '\r' )
				return 2;
			else
				return 1;
		}
	}
	return 0;
}
uint TextBlock::EOLOffset() const
{
	if( !isValid() ) return 0;
	return size() - newlineLength();
#if 0
	const size_t sz = size();
	index_t np = position() + sz;
	if( np > position() ) {
		uchar uch = (*m_document)[np-1];
		if( uch == '\r' )
			return sz - 1;
		if( uch == '\n' ) {
			if( np - 2 >= position() && (*m_document)[np-2] == '\r' )
				return sz - 2;
			else
				return sz - 1;
		}
	}
	return sz;
#endif
}

int TextBlock::charsCount(index_t position) const
{
	if( !isValid() || position <= m_data.position() ||
		position > m_data.position() + m_document->blockSize(m_data.index()) )
	{
		return 0;
	}
	int cnt = 0;
	int ix = m_data.position();
	while( ix < position ) {
		++cnt;
		ix += UTF8CharSize((*m_document)[ix]);
	}
	return cnt;
}
	
QString TextBlock::text() const
{
	if( !isValid() ) return QString();
	/*const*/ size_t sz = size();
	if( !sz ) return QString();
	sz = qMin(sz, m_document->size() - position());
	QByteArray ba;
	ba.reserve(sz);
	Q_ASSERT( position() + sz <= m_document->size() );
	for(index_t ix = position(), iend = position() + sz; ix != iend; ++ix)
		ba += (*m_document)[ix];
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
TextBlock TextBlock::next() const
{
	if( !isValid() ) return *this;
#if 1
	TextBlockData d = m_document->nextBlockData(m_data);
	if( d.index() >= m_document->blockCount() )
		d.m_index = INVALID_INDEX;
	return TextBlock(m_document, d);
#else
	index_t blockPosition = m_data.m_position + m_document->blockSize(m_data.m_index);
	int ix = m_data.m_index + 1;
	if( ix >= m_document->blockCount() )
		ix = INVALID_INDEX;
	return TextBlock(m_document, ix, blockPosition);
#endif
}
TextBlock TextBlock::prev() const
{
	if( !isValid() ) return *this;
	TextBlockData d = m_document->prevBlockData(m_data);
	return TextBlock(m_document, d);
}
//----------------------------------------------------------------------
TextDocument::TextDocument(QObject *parent)
	: QObject(parent)
{
	init();
}

TextDocument::~TextDocument()
{

}

void TextDocument::init()
{
	m_modified = false;
	m_buffer.clear();
	m_blocks.clear();
	m_blocks.push_back(TextBlockItem(0));
	m_blockData = TextBlockData(0, 0);
	//m_blockIndex = m_blockPosition = 0;
	emit blockCountChanged();
}
size_t TextDocument::blockSize(index_t ix) const
{
	if( ix >= m_blocks.size() ) return 0;
	return m_blocks[ix].m_size;
}
QString TextDocument::toPlainText() const
{
	QByteArray ba;
	ba.reserve(size());
	for(index_t ix = 0; ix != size(); ++ix)
		ba += m_buffer[ix];
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba);
}
TextBlockData TextDocument::findBlockData(index_t position) const
{
	if( m_blocks.size() == 1 )
		return TextBlockData(0, 0);
	TextBlockData data(0, 0), next;
	if( m_blockData.m_index == 0 ) {		//	�L���b�V���������ꍇ
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
index_t	TextDocument::findBlockIndex(index_t position, index_t *pBlockPos) const
{
	TextBlockData d = findBlockData(position);
	if( pBlockPos != 0 )
		*pBlockPos = d.position();
	return d.index();
}

size_t TextDocument::blockPosition(index_t ix) const
{
	size_t pos = 0;
	for(int i = 0; i < ix; ++i)
		pos += m_blocks[i].m_size;
	return pos;
}

TextBlock TextDocument::findBlock(index_t position) const
{
	if( position > size() ) return TextBlock(const_cast<TextDocument*>(this), INVALID_INDEX, 0);
	index_t blockPosition;
	//const index_t ix = findBlockIndex(position, &blockPosition);
	TextBlockData d = findBlockData(position);
	return TextBlock(const_cast<TextDocument*>(this), d);
}
TextBlock TextDocument::findBlockByNumberRaw(index_t blockIndex) const
{
	index_t blockPosition = 0;
	index_t ix = 0;
	if( blockIndex <= blockCount() / 2 ) {
		while( ix < blockIndex )
			blockPosition += m_blocks[ix++].m_size;
	} else {	//	���������̏ꍇ
		blockPosition = size();
		index_t ix = blockCount();
		while( ix > blockIndex )
			blockPosition -= m_blocks[--ix].m_size;
	}
	return TextBlock(const_cast<TextDocument*>(this), ix, blockPosition);
}
TextBlock TextDocument::findBlockByNumber(index_t blockIndex) const
{
	if( blockIndex >= blockCount() - 1 ) {
		m_blockData.m_index = blockCount() - 1;
		m_blockData.m_position = size() - blockSize(m_blockData.m_index);
		return TextBlock(const_cast<TextDocument*>(this), m_blockData);
	}
	index_t blockPosition = 0;
	index_t ix = 0;
	if( m_blockData.m_index == 0 ) {		//	�L���b�V���������ꍇ
		if( blockIndex <= blockCount() / 2 ) {
			while( ix < blockIndex )
				blockPosition += m_blocks[ix++].m_size;
		} else {	//	���������̏ꍇ
			blockPosition = size();
			ix = blockCount();
			while( ix > blockIndex )
				blockPosition -= m_blocks[--ix].m_size;
		}
	} else {
		if( blockIndex == m_blockData.m_index )
			return TextBlock(const_cast<TextDocument*>(this), m_blockData.m_index, m_blockData.m_position);
		if( blockIndex < m_blockData.m_index ) {
#if 0	//	�t�����V�[�P���V�����A�N�Z�X�p�x�͒Ⴂ�̂ŃR�����g�A�E�g���Ă���
			if( blockIndex == m_blockIndex - 1 ) {
				m_blockPosition -= m_blocks[--m_blockIndex].m_size;
				return TextBlock(this, m_blockIndex, m_blockPosition);
			}
#endif
			if( blockIndex <= m_blockData.m_index / 2 ) {
				while( ix < blockIndex )
					blockPosition += m_blocks[ix++].m_size;
			} else {	//	���������̏ꍇ
				blockPosition = m_blockData.m_position;
				ix = m_blockData.m_index;
				while( ix > blockIndex )
					blockPosition -= m_blocks[--ix].m_size;
			}
		} else {	//	m_blockData.m_index < blockIndex < m_blocks.size() �̏ꍇ
			if( blockIndex == m_blockData.m_index + 1 ) {
				m_blockData.m_position += m_blocks[m_blockData.m_index++].m_size;
				return TextBlock(const_cast<TextDocument*>(this), m_blockData);
			}
			if( blockIndex <= m_blockData.m_index + (m_blocks.size() - m_blockData.m_index) / 2 ) {
				blockPosition = m_blockData.m_position;
				ix = m_blockData.m_index;
				while( ix < blockIndex )
					blockPosition += m_blocks[ix++].m_size;
			} else {	//	���������̏ꍇ
#if 1
				blockPosition = size();
				ix = m_blocks.size();
				do {
					blockPosition -= m_blocks[--ix].m_size;
				} while( ix > blockIndex );
#else
				blockPosition = m_blockData.m_position;
				ix = m_blockData.m_index;
				while( ix > blockIndex )
					blockPosition -= m_blocks[--ix].m_size;
#endif
			}
		}
	}
	m_blockData.m_index = ix;
	m_blockData.m_position = blockPosition;
	return TextBlock(const_cast<TextDocument*>(this), ix, blockPosition);
}

void TextDocument::buildBlocks()
{
	m_blocks.clear();
	index_t offset = 0;
	index_t ix = 0;
	while( ix < m_buffer.size() ) {
		uchar ch = m_buffer[ix++];
		if( ch == '\n' ) {
			m_blocks.push_back(TextBlockItem(ix - offset));
			offset = ix;
		} else if( ch == '\r' ) {
			if( ix < m_buffer.size() && m_buffer[ix] == '\n' )
				++ix;
			m_blocks.push_back(TextBlockItem(ix - offset));
			offset = ix;
		}
	}
	//if( ix != offset )
		m_blocks.push_back(TextBlockItem(ix - offset));		//	EOF �s
	emit blockCountChanged();
}
void TextDocument::updateBlocksAtInsert(index_t first,
						TextBlockData d, /*index_t blockIndex, index_t blockPosition,*/
						size_t sz)
{
	bool bcChanged = false;
	index_t offset = first - d.position();		//	�}���ʒu����s���܂ł̕����o�C�g��
	const size_t rest = m_blocks[d.index()].m_size - offset;	//	�}���ʒu���玟�̍s���܂ł̕����o�C�g��
	index_t last = first + sz;
	uchar ch = 0;
	while( first < last ) {
		ch = m_buffer[first++];
		if( ch == '\n' ) {
		} else if( ch == '\r' ) {
			const bool isNextLF = first < size() && m_buffer[first] == '\n';
			if( first == last ) {	//	�Ō�̑}�������� \r �̏ꍇ
				if( isNextLF ) break;	//	���� \n �Ȃ�΍s���쐬���Ȃ�
			} else {
				if( isNextLF )
					++first;
			}
		} else
			continue;
		m_blocks.insert(d.index(), TextBlockItem(first - d.position()));
		bcChanged = true;
		++d.m_index;
		d.m_position = first;
	}
	if( ch != '\n' && ch != '\n' )	//	�Ō�̑}�����������s�Ŗ����ꍇ
		m_blocks[d.index()].m_size += (last - d.position()) - offset;
	else
		m_blocks[d.index()].m_size = rest;
	if( bcChanged )
		emit blockCountChanged();
}
#if 0
void TextDocument::updateBlocksAtInsert(index_t first, size_t sz)
{
	//index_t blockPos = 0;
	//index_t bix = findBlockIndex(first, &blockPos);
	TextBlockData d = findBlockData(first);
	updateBlocksAtInsert(first, bix, blockPos, sz);
}
#endif
//	erase �������s���钼�O�ɃR�[������A
//	m_blocks ���X�V
void TextDocument::updateBlocksAtErase(index_t first,
					TextBlockData d, /*index_t blockIndex, index_t blockPosition,*/
					index_t last)
{
	size_t sz = last - first;
	index_t bixLast = d.index();
	index_t offset = first - d.position();		//	�폜�ʒu����s���܂ł̕����o�C�g��
	uchar ch = 0;
	while( first < last ) {
		ch = m_buffer[first++];
		if( ch == '\n' ) {
		} else if( ch == '\r' ) {
			const bool isNextLF = first < size() && m_buffer[first] == '\n';
			if( first == last ) {	//	�Ō�̑}�������� \r �̏ꍇ
				if( isNextLF ) break;	//	���� \n �Ȃ�΍s���폜���Ȃ�
			} else {
				if( isNextLF )
					++first;
			}
		} else
			continue;
		d.m_position = first;
		++bixLast;
	}
	if( bixLast != d.index() ) {
		m_blocks.erase(d.index(), bixLast);
		emit blockCountChanged();
	}
	m_blocks[d.index()].m_size += offset - (last - d.position());
	//size_t t = m_blocks[blockIndex].m_size;	//	for debug
}
#if 0
void TextDocument::updateBlocksAtErase(index_t first, index_t last)
{
	index_t blockPosition = 0;
	index_t blockIndex = findBlockIndex(first, &blockPosition);
	updateBlocksAtErase(first, blockIndex, blockPosition, last);
}
#endif

void TextDocument::erase(index_t first, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, findBlockData(first), last);
	m_buffer.erase(first, last);
}
void TextDocument::erase(index_t first, TextBlockData d, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, d, last);
	m_buffer.erase(first, last);
}
void TextDocument::insert(index_t position, TextBlockData d, const QString &text)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	if( position > m_buffer.size() ) position = m_buffer.size();
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, d, sz);
}
void TextDocument::insert(index_t position, const QString &text)
{
	//index_t blockPosition = 0;
	//index_t blockIndex = findBlockIndex(position, &blockPosition);
	insert(position, findBlockData(position), text);
}
void TextDocument::insert(index_t ix, TextBlockData d,
							cuchar *first, cuchar *last)
{
	m_buffer.insert(ix, first, last);
	updateBlocksAtInsert(ix, d, last - first);
}
void TextDocument::insert(index_t ix, cuchar *first, cuchar *last)
{
	//index_t blockPosition = 0;
	//index_t blockIndex = findBlockIndex(ix, &blockPosition);
	insert(ix, findBlockData(ix), first, last);
}

void TextDocument::setPlainText(const QString &text)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	m_buffer.clear();
	m_buffer.insert(0, ptr, ptr + sz);
	buildBlocks();
	//appendBlocks();
}
#if 0
void TextDocument::append(const QByteArray &utf8)
{
	const int sz = utf8.length();
	const uchar *ptr = (const uchar *)(utf8.data());
	m_buffer.insert(m_buffer.end(), ptr, ptr + sz);
	buildBlocks();
}
#endif
size_t TextDocument::deleteChar(TextCursor &cur)
{
	if( cur.isNull() || cur.document() != this )
		return 0;
	ushort flags = 0;		//
	if( !cur.hasSelection() )
		cur.movePosition(TextCursor::Right, TextCursor::KeepAnchor);
	else if( cur.position() < cur.anchor() )
		cur.swapPositionAnchor();
	else
		flags = GVUNDOITEM_CUR_TAIL;	//	�����擪���疖�������Ɍ������đI������Ă����ꍇ
	index_t first = cur.anchor();
	index_t last = cur.position();
	if( first == last ) return 0;
	do_erase(first, last, flags);
	cur.copyAnchorToPosition();
	m_blockData = cur.blockData();
	m_modified = true;
	emit contentsChange(first, last - first, 0);
	emit contentsChanged();
	return last - first;
}
size_t TextDocument::deletePreviousChar(TextCursor &cur)
{
	if( cur.isNull() || cur.document() != this )
		return 0;
	if( cur.hasSelection() )
		return deleteChar(cur);
	cur.movePosition(TextCursor::Left, TextCursor::KeepAnchor);
	index_t first = cur.position();
	index_t last = cur.anchor();
	if( first == last ) return 0;
	do_erase(first, last, GVUNDOITEM_CUR_TAIL);
	cur.copyPositionToAnchor();
	m_blockData = cur.blockData();
	m_modified = true;
	emit contentsChange(first, last - first, 0);
	emit contentsChanged();
	return last - first;
}
size_t TextDocument::insertText(TextCursor &cur, const QString &text)
{
	if( cur.isNull() || cur.document() != this )
		return 0;
	const index_t position = cur.position();
	//if( position > m_buffer.size() ) position = m_buffer.size();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//index_t hp_ix = m_undoMgr.addToHeap(ptr, ptr + sz);		//	undone P �f�[�^��undo���Ɋi�[���ׂ�
	size_t delSz = 0;
	if( position == cur.anchor() ) {
		m_buffer.insert(position, ptr, ptr + sz);
		updateBlocksAtInsert(position, cur.blockData(), sz);
		m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0),
							isModified());
	} else {
		if( cur.anchor() < position )
			cur.swapPositionAnchor();
		const index_t first = cur.position();
		const index_t last = cur.anchor();
		//const index_t blockIndex = cur.blockIndex();
		//const index_t blockPosition = cur.blockPosition();
		const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
		erase(first, cur.blockData(), last);
		m_buffer.insert(first, ptr, ptr + sz);
		updateBlocksAtInsert(first, cur.blockData(), sz);
		m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_REPLACE, first, last, hp_ix, first + sz),
							isModified());
		delSz = last - first;
	}
	cur.setPosition(cur.position() + sz);
	//cur.movePosition(TextCursor::Right, TextCursor::MoveAnchor, text.length());
	m_blockData = cur.blockData();
	m_modified = true;
	emit contentsChange(position, delSz, sz);
	emit contentsChanged();
	return sz;		//	�}���o�C�g��
}
void TextDocument::do_insert(index_t position, const QString &text)
{
	if( position > m_buffer.size() ) position = m_buffer.size();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//index_t hp_ix = m_undoMgr.addToHeap(ptr, ptr + sz);		//	undone P �f�[�^��undo���Ɋi�[���ׂ�
	TextBlockData d = findBlockData(position);		//	�ҏW�ɂ��L���b�V���������ɂȂ�O�Ɏ擾���Ă���
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, d, sz);
#if 1
	m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0),
						isModified());
#else
	GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0);
	m_undoMgr.push_back(undoItem, isModified());
#endif
	m_modified = true;
	emit contentsChange(position, 0, sz);
	emit contentsChanged();
}
void TextDocument::do_erase(index_t first, index_t last, ushort flag)
{
	const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	erase(first, last);
	m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_ERASE, first, last, hp_ix, 0, flag),
						isModified());
	emit contentsChange(first, last - first, 0);
}
void TextDocument::do_replace(index_t first, index_t last, const QString &text)
{
	if( last > m_buffer.size() ) last = m_buffer.size();
	if( first >= last ) {
		do_insert(first, text);
		return;
	}
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	const index_t hp_ix = m_undoMgr.addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	TextBlockData d = findBlockData(first);
	erase(first, last);
	m_buffer.insert(first, ptr, ptr + sz);
	updateBlocksAtInsert(first, d, sz);
#if 1
	m_undoMgr.push_back(GVUndoItem(GVUNDOITEM_TYPE_REPLACE, first, last, hp_ix, first + sz),
						isModified());
#else
	GVUndoItem *undoItem = new (m_pool_undoItem.malloc()) GVUndoItem(GVUNDOITEM_TYPE_REPLACE,
									first, last, hp_ix, first + sz);
	m_undoMgr.push_back(undoItem, isModified());
#endif
	emit contentsChange(first, last - first, sz);
}
void TextDocument::doUndo(index_t &pos, index_t &anchor)
{
	if( !m_undoMgr.canUndo() ) return;
	setModified(m_undoMgr.doUndo(this, pos, anchor));
	emit contentsChanged();
}
void TextDocument::doRedo(index_t &pos, index_t &anchor)
{
	if( !m_undoMgr.canRedo() ) return;
	setModified(m_undoMgr.doRedo(this, pos, anchor));
	emit contentsChanged();
}

//	�P�����`�����A���S���Y��
bool TextDocument::isMatch(index_t position, cuchar *first, cuchar *last, ushort options) const
{
	if( (options & MatchCase) != 0 )
		return isMatch(position, first, last);
	else
		return isMatchIgnoreCase(position, first, last);
}
bool TextDocument::isMatch(index_t position, cuchar *first, cuchar *last) const
{
	while( first < last ) {
		if( *first != m_buffer[position] )
			return false;
		++first;
		++position;
	}
	return true;
}
bool TextDocument::isMatchIgnoreCase(index_t position, cuchar *first, cuchar *last) const
{
	while( first < last ) {
		if( tolower(*first) != tolower(m_buffer[position]) )
			return false;
		++first;
		++position;
	}
	return true;
}
bool TextDocument::isMatched(const QString &text, const TextCursor &cur, ushort options)
{
	if( text.isEmpty() ) return false;
	if( !cur.hasSelection() ) return false;		//	�Ƃ肠�����I��̈�Ƃ̂ݔ�r����
	const QString buf = cur.selectedText();
	if( (options & MatchCase) != 0 )
		return text == buf;
	else
		return 0 == text.compare(buf, Qt::CaseInsensitive);
}
TextCursor TextDocument::find(const QString &text, const TextCursor &cur, ushort options)
{
	if( cur.hasSelection() &&
		((options & FindBackWard) == 0 && cur.anchor() > cur.position() ||
		(options & FindBackWard) != 0 && cur.anchor() < cur.position()) )
	{
		return find(text, cur.anchor(), options);
	}
	return find(text, cur.position(), options);
}
TextCursor TextDocument::find(const QString &text, index_t position, ushort options)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//	�P�����`�����A���S���Y��
	if( (options & FindBackWard) == 0 ) {
		while( position < size() ) {
			if( isMatch(position, ptr, ptr + sz, options) ) {
				TextCursor c(this, position);
				c.setPosition(position + sz, TextCursor::KeepAnchor);
				return c;
			}
			++position;
		}
	} else {
		while( position > 0 ) {
			--position;
			if( isMatchIgnoreCase(position, ptr, ptr + sz) ) {
				TextCursor c(this, position);
				c.setPosition(position + sz, TextCursor::KeepAnchor);
				return c;
			}
		}
	}
	return TextCursor();	//	null cursor
}
