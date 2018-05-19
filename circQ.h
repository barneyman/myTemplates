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

public:

	circQueueT()
	{
		reset();
	}

	// number of bytes to READ from the queue
	unsigned available()
	{
		return availBytes;
	}

	// it's buffer size maximum
	int size()
	{
		return _CIRCQSIZE;
	}

	// size-avail = room
	int space()
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
		return m_data[readCursor + offset];
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

		{
			// relying on bits width math
			if ((readCursor - 1) != writeCursor)
			{
				m_data[writeCursor++] = data;
				availBytes++;
				ret = true;
			}
		}

		return ret;
	}

};

