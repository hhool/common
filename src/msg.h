#pragma once

#include "data.h"
#include "comm.h"

namespace Common {
	// �����ļ���ʽѡ��Ի���
	class c_send_file_format_dlg : public c_dialog_builder
	{
	public:
		c_send_file_format_dlg() {  }
		SdkLayout::CTinyString get_selected_type() { return _selected; }
	protected:
		virtual LRESULT		handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;
		virtual LPCTSTR		get_skin_xml() const override;
		virtual LRESULT		on_command_ctrl(HWND hwnd, SdkLayout::CControlUI* ctrl, int code) override;

	protected:
		SdkLayout::CTinyString _selected;
	};

	// ���͸�ʽ���ƶԻ���
	class c_send_data_format_dlg : public c_dialog_builder
	{
	public:
		c_send_data_format_dlg(bool bchar, DWORD* attr)
			: _bchar(bchar)
			, _dwAttr(attr)
		{
		}

	protected:
		virtual LRESULT		handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
		virtual LPCTSTR		get_skin_xml() const override;
		virtual LRESULT		on_command_ctrl(HWND hwnd, SdkLayout::CControlUI* ctrl, int code);
		virtual DWORD		get_window_style() const { return WS_OVERLAPPEDWINDOW; }
		virtual DWORD		get_window_ex_style() const override { return WS_EX_TOOLWINDOW; }
		virtual void		on_final_message(HWND hwnd) { __super::on_final_message(hwnd); delete this; }

	protected:
		bool _bchar;
		DWORD* _dwAttr;
	};

	class CComWnd 
		: public CWnd
		, public i_data_counter
		, public i_timer
		, public i_timer_period
		, public IAcceleratorTranslator
	{
		friend class c_send_data_format_dlg;
	public:
		CComWnd();
		~CComWnd();

	private:
		enum PrivateMessage{
			__kPrivateStart = WM_APP,
			kMessageBox,
			kUpdateCounter,
			kUpdateStatus,
			kUpdateTimer,
			kAutoSend,
		};

		// ���ͷ�ʽֻ������: 16���� + �ı��ַ�, ��_b_send_data_frmat_hex����; ���ú������������
		// ����������ѡ��, ����: �س�����, �Ƿ��������ַ���
		enum SendDataFormatHex{
			sdfh_kNone		= 0x00000000,
		};
		enum SendDataFormatChar{
			sdfc_kNone		= 0x00000000,
			// 01��λ�������з�����
			sdfc_kNoCrlf	= 0x00000000,
			sdfc_kCr		= 0x00000001,
			sdfc_kLf		= 0x00000002,
			sdfc_kCrlf		= 0x00000003,
			// ��2λ �����Ƿ�ʹ��ת���ַ�
			sdfc_kUseEscape = 0x00000004,
		};

	protected:
		// interface Deal::i_data_counter
		virtual void update_counter(long rd, long wr, long uw) override;
		// interface i_com_timer
		virtual void update_timer(int h, int m, int s) override;
		// interface 
		virtual void update_timer_period() override;

		void update_status(const char* fmt, ...);

		// IAcceleratorTranslator interface
		virtual bool TranslateAccelerator(MSG* pmsg);

		// ���ڶ��̲߳���, ������д
		virtual int msgbox(UINT msgicon, char* caption, char* fmt, ...);

	protected:
		LRESULT CALLBACK RichEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
		virtual void OnFinalMessage(HWND hWnd);

	private:
		LRESULT on_create(HWND hWnd, HINSTANCE hInstance);
		LRESULT on_scroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT on_size(int width,int height);
		LRESULT on_close();
		LRESULT on_destroy();
		LRESULT on_command(HWND hWndCtrl, int id, int codeNotify);
		LRESULT on_device_change(WPARAM event, DEV_BROADCAST_HDR* pDBH);
		LRESULT on_setting_change(WPARAM wParam, LPCTSTR area);
		LRESULT on_app(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT on_contextmenu(HWND hwnd, int x, int y);

	private:
		LRESULT on_command_menu(int id);
		LRESULT on_command_acce(int id);
		LRESULT on_command_ctrl(HWND hwnd, int id, int code);

	private:
		// һЩ�������
		void init_from_config_file();
		void save_to_config_file();

		void switch_rich_edit_fullscreen(bool full);
		void switch_window_top_most(bool manual=false, bool topmost = true);
		void switch_simple_ui(bool manual=false, bool bsimple=false);
		void switch_send_data_format(bool manual=false, bool bhex=false, DWORD fmthex=0,DWORD fmtchar=0);
		void switch_recv_data_format(bool manual = false, bool bhex = false, DWORD fmthex = 0, DWORD fmtchar = 0);
		bool is_send_data_format_hex() { return _b_send_data_format_hex; }
		bool is_send_data_format_char(){ return !is_send_data_format_hex(); }
		bool is_recv_data_format_hex() { return _b_recv_data_format_hex; }
		bool is_recv_data_format_char(){ return !is_recv_data_format_hex(); }
		void switch_auto_send(bool manual=false, bool bauto=false, int interval=-1);
		

	public:
		Window::c_edit*			editor_send()		{return &_send_edit;}
		Window::c_edit*			editor_recv_hex()	{return &_recv_hex_edit;}
		Window::c_rich_edit*	editor_recv_char()	{return &_recv_char_edit;}

	//////////////////////////////////////////////////////////////////////////
	// ���¹���������ص�һЩ����, ��: �������б� ...

	// ���ڶ���
	class t_com_item
	{
	public:
		t_com_item(int i,const char* s){_s = s; _i=i;}

		// �����ַ�������: ����: ��У��λ
		std::string get_s() const {return _s;}
		// ������������ : ����: NOPARITY(��)
		int get_i() const {return _i;}

	protected:
		std::string _s;
		int _i;
	};

	// ˢ�´��ڶ����б�ʱ��Ҫ�õ��Ļص���������
	typedef void t_list_callback(void* ud, const t_com_item* t);

	// ���ڶ���ˢ��ʱ�Ļص����ͽӿ�
	class i_com_list
	{
	public:
		virtual void callback(t_list_callback* cb, void* ud) = 0;
	};

	// ���ڶ�������: ���� ����ϵͳ���еĴ����б�
	template<class T>
	class t_com_list : public i_com_list
	{
	public:
		void empty() {_list.clear();}
		const T& add(T t) { _list.push_back(t); return _list[_list.size() - 1]; }
		int size() {return _list.size();}
		const T& operator[](int i) {return _list[i];}

		// ���¶����б�, �������ϵͳ�����б�
		virtual i_com_list* update_list(){return this;}

		virtual operator i_com_list*() {return static_cast<i_com_list*>(this);}
		virtual void callback(t_list_callback* cb, void* ud)
		{
			for(int i=0,c=_list.size(); i<c; i++){
				cb(ud, &_list[i]);
			}
		}

	protected:
		std::vector<T> _list;
	};

	// ���ڶ˿��б�, �̳е�ԭ����: �˿���һ����ν�� "�Ѻ���"
	// ���糣����: Prolific USB-to-Serial Comm Port
	// ���µ������б��ؼ���ʱ��Ҫ��������һ��
	class c_comport : public t_com_item
	{
	public:
		c_comport(int id,const char* s)
			: t_com_item(id, s)
		{}

		std::string get_id_and_name() const;
	};

	// ���ڶ˿�����: Ҫ��ϵͳȡ���б�, ������д
	class c_comport_list : public t_com_list<c_comport>
	{
	public:
		virtual i_com_list* update_list();
	};

	// ���ڲ����ʿ����ⲿ�ֶ�����, ���Զ��һ����Ա
	class c_baudrate : public t_com_item
	{
	public:
		c_baudrate(int id, const char* s, bool inner)
			: t_com_item(id, s)
			, _inner(inner)
		{}

		bool is_added_by_user() const { return !_inner; }
	protected:
		bool _inner;
	};

	// ���ڶ����б�
	private:
		c_comport_list			_comport_list;
		t_com_list<c_baudrate>	_baudrate_list;
		t_com_list<t_com_item>	_parity_list;
		t_com_list<t_com_item>	_stopbit_list;
		t_com_list<t_com_item>	_databit_list;

	private:
		struct list_callback_ud{
			enum e_type{
				cp,br,pa,sb,db
			}type;
			CComWnd* that;
			HWND hwnd;
		};
		static void com_udpate_list_callback(void* ud, const t_com_item* t);
		void com_update_item_list();
		void com_update_comport_list();
		void com_update_comport_list_and_select_current();
		bool com_flush_settings_from_combobox();
		bool com_try_close(bool b_thread_started);
		bool com_try_open();
		void com_openclose();
		void com_lock_ui_panel(bool lock);
		void com_add_prompt_if_no_cp_presents();
		void com_update_open_btn_text();
		void com_copy_text_data_to_clipboard(HWND hwnd);
		void com_load_file();
		bool _com_load_file_prompt_size(SdkLayout::CTinyString& selected, c_binary_file& bf);
		bool com_do_send(bool callfromautosend);

		HWND _hCP, _hBR, _hPA, _hSB, _hDB;
		HWND _hStatus, _hOpen;
		Window::c_rich_edit	_recv_char_edit;
		bool				_b_recv_char_edit_fullscreen;
		Window::c_edit		_recv_hex_edit;
		Window::c_edit		_send_edit;
		sdklayout*			m_layout;
		HACCEL				m_hAccel;

		CComm				_comm;
		c_timer				_timer;
		c_timer				_auto_send_timer;

		char				_send_buffer[10240];	//����Ĭ��ȡ�������ݿ������
		Window::c_edit*		_recv_cur_edit;			// ��ǰ�������ݸ�ʽ�ı��ؼ�
		bool				_b_send_data_format_hex;
		bool				_b_recv_data_format_hex;
		DWORD				_send_data_format_hex;
		DWORD				_send_data_format_char;

		HexReceiver		_hex_data_receiver;
		TextReceiver	_text_data_receiver;
		FileReceiver	_file_data_receiver;

		AThunk				_thunk_rich_edit;
		WNDPROC				_thunk_rich_edit_old_proc;

		c_observable		_window_close_handler;

        // �Ƿ���Ҫ�ڹرպ�ˢ�´����б�����Ϊ���ڴ�ʱ�޷��޸��б�������~
        bool                _b_refresh_comport;
	};
}
