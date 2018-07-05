#include <inttypes.h>


template <int _CIRCQSIZE, int _CIRQSIZEBITS, typename STORED=unsigned char, typename COUNTERS=unsigned char>
class circQueueT
{


protected:

	typedef STORED qStorageType;
	typedef COUNTERS qCounterType;

	qStorageType m_data[_CIRCQSIZE];
	// let the compiler do the work
	volatile qCounterType readCursor : _CIRQSIZEBITS, writeCursor : _CIRQSIZEBITS;
	volatile qCounterType readCursorState : _CIRQSIZEBITS, writeCursorState : _CIRQSIZEBITS;
	volatile qCounterType availBytes, availBytesState;
	bool m_overWriteWhenFull;

public:

	// if true, the queue will continue at full, overwrite and bump the readcursor - data gets lost
	circQueueT(bool keepWriting=false):m_overWriteWhenFull(keepWriting)
	{
		reset();
	}

	// number of bytes to READ from the queue
	qCounterType available()
	{
		return availBytes;
	}

	// its buffer size maximum
	qCounterType size()
	{
		return _CIRCQSIZE;
	}

	// size-avail = room
	qCounterType space()
	{
		return _CIRCQSIZE - available();
	}

	virtual void reset()
	{
		readCursor = writeCursor = availBytes = 0;
		readCursorState = writeCursorState = availBytesState = 0;
	}

	void pushState()
	{
		readCursorState = readCursor;
		writeCursorState = writeCursor;
		availBytesState = availBytes;
	}

	void popState()
	{
		readCursor = readCursorState;
		writeCursor = writeCursorState;
		availBytes = availBytesState;
	}

	qStorageType peek(qCounterType offset)
	{
		// up to the caller to check available
		// otherwise this will give you stale
		
		// bit arithmatic should handle wrap
		return m_data[(readCursor + offset)%_CIRCQSIZE];
	}

	// shorthand for peek
	qStorageType operator[](qCounterType offset)
	{
		return peek(offset);
	}

	qStorageType read()
	{
		qStorageType ret = -1;

		if (availBytes)
		{
			// 
			availBytes--;
			ret = m_data[readCursor++];
		}

		return ret;
	}

	bool write(qStorageType data)
	{
		bool ret = false;

		// if we overwrite, make space
		if (m_overWriteWhenFull && !space())
			read();

		// relying on bits width math
		if (space())
		{
			m_data[writeCursor++] = data;
			availBytes++;
			ret = true;
		}

		return ret;
	}


};

