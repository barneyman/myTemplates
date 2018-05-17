
template <int _CIRCQSIZE, int _CIRQSIZEBITS>
class circQueueT
{

protected:

	byte m_data[_CIRCQSIZE];
	// let the compiler do the work
	volatile byte readCursor : _CIRQSIZEBITS, writeCursor : _CIRQSIZEBITS;
	volatile byte readCursorState : _CIRQSIZEBITS, writeCursorState : _CIRQSIZEBITS;
	volatile byte availBytes, availBytesState;

public:

	circQueueT()
	{
		reset();
	}

	unsigned available()
	{
		return availBytes;
	}

	int size()
	{
		return _CIRCQSIZE;
	}

	int space()
	{
		return _CIRCQSIZE - available();
	}

	void reset()
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



	byte read()
	{
		byte ret = -1;

		{
			if (readCursor != writeCursor)
			{
				// 
				availBytes--;
				ret = m_data[readCursor++];
			}

		}

		return ret;
	}

	bool write(byte data)
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

