#pragma once

namespace Common{
	// ���ݴ������ӿ�: �����ı������� 16���ƹ�����, ����������ݽ���������
	// һ������Ҫ�к������������ݴ����̳д˽ӿ�, �������ֱ�Ӵ���, ����'\t'�Ĵ����Ͳ���Ҫ
	class IDataProcessor
	{
	public:
		// ������������: 
		//		follow:	ǰһ�ε����Ƿ�Ϊ����������, Ҳ��������������
		//		ba:		Byte Array, �ֽ�����
		//		cb:		Count of Bytes, �ֽ���
		//		*pn:	���δ����˶�������
		// ����ֵ:
		//		bool:	�Ƿ�ϣ����������, Ӱ����һ�ε���ʱfollow��ֵ
		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn) = 0;

		// �������ݴ�������: ����, �ڹرմ��ں�, �����16�������ݺ�
		virtual void reset_buffer() = 0;
	};

	// ���ݽ������ӿ�: �����ڽ��յ����ݺ�������еĽ�����
	class IDataReceiver
	{
	public:
		// ���ݽ��պ���, ���߳̽��յ�����ʱ���ô˺���
		// baָ������ݲ�Ӧ�ñ�����!
		virtual void receive(const unsigned char* ba, int cb) = 0;
		virtual void reset_buffer() = 0;
	protected:
		// һ�����ô�����������ʣ��������������ñ�־�ĸ�������
		virtual bool process(IDataProcessor* proc, bool follow, const unsigned char** pba, int* pcb, IDataProcessor** ppre)
		{
			int n;
			bool c = proc->process_some(follow, *pba, *pcb, &n);
			SMART_ASSERT(n <= *pcb)(n)(*pcb).Fatal();
			*pba += n;
			*pcb -= n;
			*ppre = c ? proc : NULL;
			return c;
		}
	};

	class SingleByteProcessor : public IDataProcessor
	{
	public:
		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn);
		virtual void reset_buffer() {}

	public:
		Window::c_rich_edit*	_richedit;
	};

	class NewlineProcessor : public IDataProcessor
	{
	public:
		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn);
		virtual void reset_buffer();

		NewlineProcessor()
			: _post_len(0)
		{}

	protected:
		int _post_len;
		c_byte_array<16, 64> _data;
	public:
		Window::c_rich_edit* _richedit;
	};

	// Linux�����ַ�����
	// http://www.cnblogs.com/memset/p/linux_printf_with_color.html
	// http://ascii-table.com/ansi-escape-sequences.php
	class EscapeProcessor : public IDataProcessor
	{
	public:
		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn);
		virtual void reset_buffer();

	protected:
		enum lcs_state{
			LCS_NONE,LCS_ESC,LCS_BRACKET,LCS_VAL,LCS_SEMI,
			LCS_H,LCS_f,
			LCS_A,LCS_B,LCS_C,LCS_D,
			LCS_s,LCS_u,LCS_j,LCS_K,
			LCS_h,LCS_l,LCS_EQU,
			LCS_m,LCS_P
		} _state;
		c_byte_array<64, 64> _data;		// ����ջ
		std::vector<lcs_state> _stack;	// ״̬ջ
	public:
		Window::c_rich_edit* _richedit;
	};

	class AsciiProcessor : public IDataProcessor
	{
	public:
		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn);
		virtual void reset_buffer();

	public:
		Window::c_rich_edit* _richedit;
	};

	// ������չASCII�ַ����� (CodePage936 compatible, EUC-CN)
	// http://en.wikipedia.org/wiki/GB_2312
	// http://zh.wikipedia.org/wiki/GB_2312
	// http://www.knowsky.com/resource/gb2312tbl.htm
	class Gb2312Processor : public IDataProcessor
	{
	public:
		Gb2312Processor()
			: _lead_byte(0)
		{}
		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn);
		virtual void reset_buffer();

	public:
		unsigned char _lead_byte;			// ����ǰ���ַ�
		Window::c_rich_edit* _richedit;
	};

	//////////////////////////////////////////////////////////////////////////
	class TextReceiver : public IDataReceiver
	{
	public:
		TextReceiver()
			: _pre_proc(0)
			, _rich_editor(0)
		{}

		// interface IDataReceiver
		virtual void receive(const unsigned char* ba, int cb);
		virtual void reset_buffer(){
			_pre_proc = 0;
			_proc_ascii.reset_buffer();
			_proc_escape.reset_buffer();
			_proc_crlf.reset_buffer();
			_proc_gb2312.reset_buffer();
		}
		void set_editor(Window::c_rich_edit* edt) {
			_rich_editor = edt;
			_proc_byte._richedit = edt;
			_proc_ascii._richedit = edt;
			_proc_escape._richedit = edt;
			_proc_crlf._richedit = edt;
			_proc_gb2312._richedit = edt;
		}

	protected:
		Window::c_rich_edit*	_rich_editor;
		IDataProcessor*			_pre_proc;
		SingleByteProcessor		_proc_byte;
		NewlineProcessor		_proc_crlf;
		EscapeProcessor		    _proc_escape;
		AsciiProcessor		    _proc_ascii;
		Gb2312Processor		    _proc_gb2312;
	};

	//////////////////////////////////////////////////////////////////////////
	class HexProcessor : public IDataProcessor
	{
	public:
		HexProcessor()
			: _editor(0)
			, _count(0)
		{}

		virtual bool process_some(bool follow, const unsigned char* ba, int cb, int* pn);
		virtual void reset_buffer();
		void set_count(int n) { _count = n; }

		Window::c_edit*			_editor;

	private:
		int _count;
	};

	class HexReceiver : public IDataReceiver
	{
	public:
		HexReceiver()
			: _pre_proc(0)
			, _editor(0)
		{}

		virtual void receive(const unsigned char* ba, int cb);
		virtual void reset_buffer(){
			_pre_proc = 0;
			_proc_hex.reset_buffer();
		}
		void set_editor(Window::c_edit* edt) {
			_editor = edt;
			_proc_hex._editor = edt;
		}

		void set_count(int n){ _proc_hex.set_count(n); }
	protected:
		Window::c_edit*		_editor;
		IDataProcessor*		_pre_proc;
		HexProcessor	    _proc_hex;
	};

	//////////////////////////////////////////////////////////////////////////
	class FileReceiver : public IDataReceiver {
	public:
		FileReceiver()
		{}

		virtual void receive(const unsigned char* ba, int cb) {
			_data.append(ba, cb);
		}
		virtual void reset_buffer() {
			_data.empty();
		}

		size_t size() { 
			return _data.get_size(); 
		}
		unsigned char* data() {
			return reinterpret_cast<unsigned char*>(_data.get_data());
		}
	protected:
		c_byte_array<1 << 20, 1 << 20> _data;
	};
}
