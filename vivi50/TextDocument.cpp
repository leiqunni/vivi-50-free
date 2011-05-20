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
#include "viCursor.h"

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
	if( m_toSetBlockFlag )
		return;		//	openUndoBlock() ��Ɉ�x�� UndoItem ���ǉ�����Ȃ������ꍇ
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
void GVUndoMgr::setDoNotMerge()
{
	if( m_items.empty() || !m_current )
		return;
	m_items[m_current - 1].m_flags |= GVUNDOITEM_DO_NOT_MERGE;
}
bool GVUndoMgr::doMergeIfPossible(const GVUndoItem &item)
{
	if( m_items.empty() ||
		(item.m_type != GVUNDOITEM_TYPE_INSERT && item.m_type != GVUNDOITEM_TYPE_REPLACE) )
	{
		return false;
	}
	GVUndoItem &last = m_items[m_items.size() - 1];
	if( (last.m_flags & GVUNDOITEM_DO_NOT_MERGE) != 0 )
		return false;
	//	undone B �s�P�ʂ̏ꍇ�͉��s�`�F�b�N
	uchar uch;
	//	�u���ɑ}���������ꍇ���}�[�W
	if( last.m_type != item.m_type && item.m_type != GVUNDOITEM_TYPE_INSERT ||
		last.m_last != item.m_first ||
		(uch = document()->at(last.m_last - 1)) == '\r' || uch == '\n' )
	{
		return false;
	}
	last.m_last = item.m_last;
	if( item.m_type == GVUNDOITEM_TYPE_REPLACE ) {
		last.m_last2 = item.m_last2;
		//	�u����������i�[���Ă���q�[�v�͘A�����Ă���͂��Ȃ̂ŁA
		//	�q�[�v�A�h���X�ɂ��Ă͉�����������K�v�͖����͂�
	}
	return true;
}
#if 0
bool GVUndoMgr::canMerge(const GVUndoItem &item) const
{
	if( m_items.empty() || item.m_type != GVUNDOITEM_TYPE_INSERT )
		return false;
	const GVUndoItem &last = m_items[m_items.size() - 1];
	return last.m_type == GVUNDOITEM_TYPE_INSERT &&
			last.m_last == item.m_first;
}
void GVUndoMgr::doMerge(const GVUndoItem &item)
{
}
#endif
#if 1
void GVUndoMgr::push_back(const GVUndoItem &item, bool modified)
{
	if( m_current < m_items.size() ) {
		//	undone M �s�v�ȃq�[�v�̉��
		m_items.erase(m_items.begin() + m_current, m_items.end());
	}
	//	undone B �}�[�W�\�ł���Β��O�A�C�e���ƃ}�[�W
	if( !doMergeIfPossible(item) )
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
	for(size_t ix = 0; ix < m_items.size(); ++ix) {
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
bool GVUndoMgr::doUndo(/*TextDocument *bb,*/ index_t& pos, index_t& anchor)
{
	if( !m_current ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[--m_current];
	GVUndoItem *ptr;
	for(bool b = true;;) {
		ptr = &m_items[--m_current];
		switch( ptr->m_type ) {
		case GVUNDOITEM_TYPE_ERASE: {
			cuchar *heap = &m_heap[ptr->m_hp_ix];
			document()->insert(ptr->m_first, heap, heap + ptr->data_size());
			if( (ptr->m_flags & GVUNDOITEM_CUR_TAIL) != 0 )
				pos = (anchor = ptr->m_first) + ptr->data_size();
			else
				anchor = (pos = ptr->m_first) + ptr->data_size();
			break;
		}
		case GVUNDOITEM_TYPE_INSERT:
			if( ptr->m_rhp_ix == 0 )
				ptr->m_rhp_ix = addToRedoHeap(document()->begin() + ptr->m_first, document()->begin() + ptr->m_last);
			document()->erase(ptr->m_first, ptr->m_last);
			pos = anchor = ptr->m_first;
			break;
		case GVUNDOITEM_TYPE_REPLACE: {
			if( ptr->m_rhp_ix == 0 )
				ptr->m_rhp_ix = addToRedoHeap(document()->begin() + ptr->m_first, document()->begin() + ptr->m_last);
			document()->erase(ptr->m_first, ptr->m_last);
			cuchar *heap = &m_heap[ptr->m_hp_ix];
			document()->insert(ptr->m_first, heap, heap + ptr->data_size2());
			if( (ptr->m_flags & GVUNDOITEM_CUR_TAIL) != 0 )
				pos = (anchor = ptr->m_first) + ptr->data_size2();
			else
				anchor = (pos = ptr->m_first) + ptr->data_size2();
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
bool GVUndoMgr::doRedo(/*TextDocument *bb,*/ index_t& pos, index_t& anchor)
{
	if( m_current >= m_items.size() ) return false;
	//boost::shared_ptr<GVUndoItem> undoItem = m_items[m_current];
	//++m_current;
	GVUndoItem *ptr;
	for(bool b = true;;) {
		ptr = &m_items[m_current++];
		switch( ptr->m_type ) {
		case GVUNDOITEM_TYPE_ERASE:
			document()->erase(ptr->m_first, ptr->m_last);
			pos = anchor = ptr->m_first;
			break;
		case GVUNDOITEM_TYPE_INSERT: {
			cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
			document()->insert(ptr->m_first, heap, heap + ptr->data_size());
			pos = anchor = ptr->m_first + ptr->data_size();
			break;
		}
		case GVUNDOITEM_TYPE_REPLACE: {
			document()->erase(ptr->m_first, ptr->m_last2);
			cuchar *heap = &m_redoHeap[ptr->m_rhp_ix];
			document()->insert(ptr->m_first, heap, heap + ptr->data_size());
			pos = (anchor = ptr->m_first) + ptr->data_size();
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
//----------------------------------------------------------------------
TextDocument::TextDocument(QObject *parent)
	: QObject(parent)
{
	m_undoMgr = new GVUndoMgr(this);
	init();
}

TextDocument::~TextDocument()
{
	delete m_undoMgr;
}

void TextDocument::init()
{
	m_modified = false;
	m_EOLCode = EOL_UNKNOWN;
	m_buffer.clear();
	m_blocks.clear();
	m_blocks.push_back(DocBlockItem(0));
	m_cacheBlockData = BlockData(0, 0);
	//m_blockIndex = m_blockPosition = 0;
	emit blockCountChanged();
}
//	�ŏ��ɂ݂������s�R�[�h���f�t�H���g�Ƃ���
void TextDocument::setEOLCode()
{
	index_t limit = qMin(size(), (index_t)0x10000);	//	�ő�64K�o�C�g�`�F�b�N
	for(index_t pos = 0; pos < limit; ++pos) {
		uchar uch = m_buffer[pos++];
		if( uch == '\n' ) {
			m_EOLCode = EOL_LF;
			return;
		} else if( uch == '\r' ) {
			if( pos < limit && m_buffer[pos] == '\n' )
				break;
			m_EOLCode = EOL_CR;
			return;
		}
	}
	m_EOLCode = EOL_CRLF;
}
QChar TextDocument::charAt(index_t ix) const
{
	if( ix >= size() ) return QChar();
	QByteArray ba;
	ba.reserve(3);
	size_t sz = UTF8CharSize(at(ix));
	while( sz != 0 ) {
		ba += at(ix++);
		--sz;
	}
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec->toUnicode(ba)[0];
}
QString TextDocument::EOLText() const
{
	switch( m_EOLCode ) {
	case EOL_CR:	return QString("\r");
	case EOL_LF:	return QString("\n");
	//case EOL_UNKNOWN:
	//case EOL_CRLF:	
	}
	return QString("\r\n");
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
BlockData TextDocument::findBlockData(index_t position) const
{
	//if( position >= size() )
	//	return BlockData(blockCount(), size());
	if( m_blocks.size() == 1 )
		return BlockData(0, 0);
	BlockData data(0, 0), next;
	if( m_cacheBlockData.m_index == 0 ) {		//	�L���b�V���������ꍇ
		if( position <= size() / 2 ) {
			while( data.m_index < m_blocks.size() - 1 &&
					position >= (next = nextBlockData(data)).position() )
				data = next;
		} else {
			data = BlockData(m_blocks.size(), size());
			do {
				data = prevBlockData(data);
			} while( data.position() > position );
		}
	} else {
		if( position < m_cacheBlockData.position() ) {
			if( position <= m_cacheBlockData.position() / 2 ) {
				while( data.m_index < m_cacheBlockData.index() - 1 &&
						position >= (next = nextBlockData(data)).position() )
					data = next;
			} else {
				data = m_cacheBlockData;
				do {
					data = prevBlockData(data);
				} while( data.position() > position );
			}
		} else {
			next = nextBlockData(m_cacheBlockData);
			if( m_cacheBlockData.position() <= position && position < next.position() )
				return m_cacheBlockData;
			if( position <= m_cacheBlockData.position() + (size() - m_cacheBlockData.position()) / 2 ) {
				while( data.m_index < m_blocks.size() - 1 &&
						position >= (next = nextBlockData(data)).position() )
					data = next;
			} else {
				data = BlockData(m_blocks.size(), size());
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
	BlockData d = findBlockData(position);
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

DocBlock TextDocument::findBlock(index_t position) const
{
	if( position > size() ) return DocBlock(const_cast<TextDocument*>(this), INVALID_INDEX, 0);
	index_t blockPosition;
	//const index_t ix = findBlockIndex(position, &blockPosition);
	BlockData d = findBlockData(position);
	return DocBlock(const_cast<TextDocument*>(this), d);
}
DocBlock TextDocument::findBlockByNumberRaw(index_t blockIndex) const
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
	return DocBlock(const_cast<TextDocument*>(this), ix, blockPosition);
}
DocBlock TextDocument::findBlockByNumber(index_t blockIndex) const
{
	if( blockIndex >= blockCount() - 1 ) {
		m_cacheBlockData.m_index = blockCount() - 1;
		m_cacheBlockData.m_position = size() - blockSize(m_cacheBlockData.m_index);
		return DocBlock(const_cast<TextDocument*>(this), m_cacheBlockData);
	}
	index_t blockPosition = 0;
	index_t ix = 0;
	if( m_cacheBlockData.m_index == 0 ) {		//	�L���b�V���������ꍇ
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
		if( blockIndex == m_cacheBlockData.m_index )
			return DocBlock(const_cast<TextDocument*>(this), m_cacheBlockData.m_index, m_cacheBlockData.m_position);
		if( blockIndex < m_cacheBlockData.m_index ) {
#if 0	//	�t�����V�[�P���V�����A�N�Z�X�p�x�͒Ⴂ�̂ŃR�����g�A�E�g���Ă���
			if( blockIndex == m_blockIndex - 1 ) {
				m_blockPosition -= m_blocks[--m_blockIndex].m_size;
				return DocBlock(this, m_blockIndex, m_blockPosition);
			}
#endif
			if( blockIndex <= m_cacheBlockData.m_index / 2 ) {
				while( ix < blockIndex )
					blockPosition += m_blocks[ix++].m_size;
			} else {	//	���������̏ꍇ
				blockPosition = m_cacheBlockData.m_position;
				ix = m_cacheBlockData.m_index;
				while( ix > blockIndex )
					blockPosition -= m_blocks[--ix].m_size;
			}
		} else {	//	m_cacheBlockData.m_index < blockIndex < m_blocks.size() �̏ꍇ
			if( blockIndex == m_cacheBlockData.m_index + 1 ) {
				m_cacheBlockData.m_position += m_blocks[m_cacheBlockData.m_index++].m_size;
				return DocBlock(const_cast<TextDocument*>(this), m_cacheBlockData);
			}
			if( blockIndex <= m_cacheBlockData.m_index + (m_blocks.size() - m_cacheBlockData.m_index) / 2 ) {
				blockPosition = m_cacheBlockData.m_position;
				ix = m_cacheBlockData.m_index;
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
				blockPosition = m_cacheBlockData.m_position;
				ix = m_cacheBlockData.m_index;
				while( ix > blockIndex )
					blockPosition -= m_blocks[--ix].m_size;
#endif
			}
		}
	}
	m_cacheBlockData.m_index = ix;
	m_cacheBlockData.m_position = blockPosition;
	return DocBlock(const_cast<TextDocument*>(this), ix, blockPosition);
}

void TextDocument::buildBlocks()
{
	m_blocks.clear();
	index_t offset = 0;
	index_t ix = 0;
	while( ix < m_buffer.size() ) {
		uchar ch = m_buffer[ix++];
		if( ch == '\n' ) {
			m_blocks.push_back(DocBlockItem(ix - offset));
			offset = ix;
		} else if( ch == '\r' ) {
			if( ix < m_buffer.size() && m_buffer[ix] == '\n' )
				++ix;
			m_blocks.push_back(DocBlockItem(ix - offset));
			offset = ix;
		}
	}
	//if( ix != offset )
		m_blocks.push_back(DocBlockItem(ix - offset));		//	EOF �s
	emit blockCountChanged();
}
void TextDocument::updateBlocksAtInsert(index_t first,
						BlockData d, /*index_t blockIndex, index_t blockPosition,*/
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
		m_blocks.insert(d.index(), DocBlockItem(first - d.position()));
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
	BlockData d = findBlockData(first);
	updateBlocksAtInsert(first, bix, blockPos, sz);
}
#endif
//	erase �������s���钼�O�ɃR�[������A
//	m_blocks ���X�V
void TextDocument::updateBlocksAtErase(index_t first,
					BlockData d, /*index_t blockIndex, index_t blockPosition,*/
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
#ifdef	_DEBUG
	size_t t = m_blocks[d.index()].m_size;	//	for debug
#endif
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
	BlockData d = findBlockData(first);
	updateBlocksAtErase(first, d, last);
	m_buffer.erase(first, last);
	m_cacheBlockData = d;	//	�L���b�V���X�V
}
void TextDocument::erase(index_t first, BlockData d, index_t last)
{
	if( last > size() ) last = size();
	updateBlocksAtErase(first, d, last);
	m_buffer.erase(first, last);
	m_cacheBlockData = d;	//	�L���b�V���X�V
}
void TextDocument::insert(index_t position, BlockData d, const QString &text)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	if( position > m_buffer.size() ) position = m_buffer.size();
	m_buffer.insert(position, ptr, ptr + sz);
	m_cacheBlockData = d;	//	�L���b�V���X�V
	updateBlocksAtInsert(position, d, sz);
}
void TextDocument::insert(index_t position, const QString &text)
{
	//index_t blockPosition = 0;
	//index_t blockIndex = findBlockIndex(position, &blockPosition);
	insert(position, findBlockData(position), text);
}
void TextDocument::insert(index_t ix, BlockData d,
							cuchar *first, cuchar *last)
{
	m_buffer.insert(ix, first, last);
	m_cacheBlockData = d;	//	�L���b�V���X�V
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
	setEOLCode();
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
size_t TextDocument::deleteChar(DocCursor &cur)
{
	if( cur.isNull() || cur.document() != this )
		return 0;
	ushort flags = 0;		//
	if( !cur.hasSelection() )
		cur.movePosition(DocCursor::Right, DocCursor::KeepAnchor);
	else if( cur.position() < cur.anchor() )
		cur.swapPositionAnchor();
	else
		flags = GVUNDOITEM_CUR_TAIL;	//	�����擪���疖�������Ɍ������đI������Ă����ꍇ
	index_t first = cur.anchor();
	index_t last = cur.position();
	if( first == last ) return 0;
	do_erase(first, last, flags);
	cur.copyAnchorToPosition();
	m_cacheBlockData = cur.blockData();
	m_modified = true;
	emit contentsChange(first, last - first, 0);
	emit contentsChanged();
	return last - first;
}
size_t TextDocument::deletePreviousChar(DocCursor &cur)
{
	if( cur.isNull() || cur.document() != this )
		return 0;
	if( cur.hasSelection() )
		return deleteChar(cur);
	cur.movePosition(DocCursor::Left, DocCursor::KeepAnchor);
	index_t first = cur.position();
	index_t last = cur.anchor();
	if( first == last ) return 0;
	do_erase(first, last, GVUNDOITEM_CUR_TAIL);
	cur.copyPositionToAnchor();
	m_cacheBlockData = cur.blockData();
	m_modified = true;
	emit contentsChange(first, last - first, 0);
	emit contentsChanged();
	return last - first;
}
int TextDocument::insertText(DocCursor &cur, const QString &text,
								bool select)		//	�}���͈͂�I��
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
		m_undoMgr->push_back(GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0),
							isModified());
	} else {
		if( cur.anchor() < position )
			cur.swapPositionAnchor();
		const index_t first = cur.position();
		const index_t last = cur.anchor();
		//const index_t blockIndex = cur.blockIndex();
		//const index_t blockPosition = cur.blockPosition();
		const index_t hp_ix = m_undoMgr->addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
		erase(first, cur.blockData(), last);
		m_buffer.insert(first, ptr, ptr + sz);
		updateBlocksAtInsert(first, cur.blockData(), sz);
		m_undoMgr->push_back(GVUndoItem(GVUNDOITEM_TYPE_REPLACE, first, first + sz, hp_ix, last),
							isModified());
		delSz = last - first;
	}
	m_cacheBlockData = cur.blockData();		//	�L���b�V���X�V
	if( select ) {
		cur.setAnchor(cur.position());
		cur.setPosition(cur.position() + sz, DocCursor::KeepAnchor);
	} else
		cur.setPosition(cur.position() + sz);
	//cur.movePosition(DocCursor::Right, DocCursor::MoveAnchor, text.length());
	//m_cacheBlockData = cur.blockData();
	m_modified = true;
	emit contentsChange(position, delSz, sz);
	emit contentsChanged();
	return sz - delSz;		//	�}���o�C�g�� - �폜�o�C�g��
}
void TextDocument::do_insert(index_t position, const QString &text)
{
	if( position > m_buffer.size() ) position = m_buffer.size();
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//index_t hp_ix = m_undoMgr.addToHeap(ptr, ptr + sz);		//	undone P �f�[�^��undo���Ɋi�[���ׂ�
	BlockData d = findBlockData(position);		//	�ҏW�ɂ��L���b�V���������ɂȂ�O�Ɏ擾���Ă���
	m_buffer.insert(position, ptr, ptr + sz);
	updateBlocksAtInsert(position, d, sz);
#if 1
	m_undoMgr->push_back(GVUndoItem(GVUNDOITEM_TYPE_INSERT, position, position + sz, 0),
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
	const index_t hp_ix = m_undoMgr->addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	erase(first, last);
	m_undoMgr->push_back(GVUndoItem(GVUNDOITEM_TYPE_ERASE, first, last, hp_ix, 0, flag),
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
	const index_t hp_ix = m_undoMgr->addToHeap(m_buffer.begin() + first, m_buffer.begin() + last);
	BlockData d = findBlockData(first);
	erase(first, last);
	m_buffer.insert(first, ptr, ptr + sz);
	updateBlocksAtInsert(first, d, sz);
#if 1
	m_undoMgr->push_back(GVUndoItem(GVUNDOITEM_TYPE_REPLACE, first, first + sz, hp_ix, last),
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
	if( !m_undoMgr->canUndo() ) return;
	setModified(m_undoMgr->doUndo(/*this,*/ pos, anchor));
	emit contentsChanged();
}
void TextDocument::doRedo(index_t &pos, index_t &anchor)
{
	if( !m_undoMgr->canRedo() ) return;
	setModified(m_undoMgr->doRedo(/*this,*/ pos, anchor));
	emit contentsChanged();
}
void TextDocument::setDoNotMergeUndoItem()
{
	m_undoMgr->setDoNotMerge();
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
bool TextDocument::isMatched(const QString &text, const DocCursor &cur, ushort options)
{
	if( text.isEmpty() ) return false;
	if( !cur.hasSelection() ) return false;		//	�Ƃ肠�����I��̈�Ƃ̂ݔ�r����
	QRegExp rex(text,
				(options & MatchCase) ? Qt::CaseSensitive : Qt::CaseInsensitive,
				(options & RegExp) ? QRegExp::RegExp : QRegExp::FixedString);
	if( !rex.isValid() ) {
		emit showMessage(tr("invalid regexp."));
		return false;
	}
	const QString buf = cur.selectedText();
	return rex.exactMatch(buf);
#if 0
	if( (options & MatchCase) != 0 )
		return text == buf;
	else
		return 0 == text.compare(buf, Qt::CaseInsensitive);
#endif
}
DocCursor TextDocument::find(const QString &text, const DocCursor &cur, ushort options)
{
	if( cur.hasSelection() &&
		((options & FindBackWard) == 0 && cur.anchor() > cur.position() ||
		(options & FindBackWard) != 0 && cur.anchor() < cur.position()) )
	{
		return find(text, cur.anchor(), options);
	}
	return find(text, cur.position(), options);
}
DocCursor TextDocument::find(const QString &text, index_t position, ushort options)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(text);
	return find(ba, position, options);
}
DocCursor TextDocument::find(const QByteArray &ba, index_t position, ushort options)
{
	const int sz = ba.length();
	const uchar *ptr = (const uchar *)(ba.data());
	//	�P�����`�����A���S���Y��
	if( (options & FindBackWard) == 0 ) {
		while( position < size() ) {
			if( isMatch(position, ptr, ptr + sz, options) ) {
				DocCursor c(this, position);
				c.setPosition(position + sz, DocCursor::KeepAnchor);
				return c;
			}
			++position;
		}
	} else {
		while( position > 0 ) {
			--position;
			if( isMatch(position, ptr, ptr + sz, options) ) {
				DocCursor c(this, position);
				c.setPosition(position + sz, DocCursor::KeepAnchor);
				return c;
			}
		}
	}
	return DocCursor();	//	null cursor
}
void TextDocument::doReplaceAll(const QString &findText, ushort options,
							const QString &replaceText)
{
	QRegExp rex(findText,
				(options & MatchCase) ? Qt::CaseSensitive : Qt::CaseInsensitive,
				(options & RegExp) ? QRegExp::RegExp : QRegExp::FixedString);
	if( !rex.isValid() ) {
		emit showMessage(tr("invalid regexp."));
		return;
	}
	openUndoBlock();
#if 1
	DocCursor cur(this);
#else
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QByteArray ba = codec->fromUnicode(findText);
	for(index_t position = 0;;) {
		DocCursor cur = find(ba, position, options);
		if( cur.isNull() ) break;
		//	undone P �u���e�L�X�g��UTF8�ɂ��炩���ߕϊ����Ă���
		insertText(cur, replaceText);
		position = cur.position();
	}
#endif
	closeUndoBlock();
}
