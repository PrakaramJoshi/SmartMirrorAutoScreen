#include<chrono>
#include <string>

namespace MirrorStateInternal{
	class MirrorState{
	protected:
		MirrorState *m_next;
	public:
		virtual void init() =0;

		void set_next_state(MirrorState *_next){
			m_next = _next;
		}

		virtual MirrorState* wait_for_state() const =0;
		virtual ~MirrorState(){
		}
	};

	class MirrorStateOn:public MirrorState{
		std::chrono::system_clock::time_point m_on_time;

		bool max_on_time_reached()const;
		void wait_for_minimum_shutoff_delay()const;

		public:

		MirrorStateOn();
		void init();
		MirrorState* wait_for_state()const;

	};

	class MirrorStateOff:public MirrorState{
		bool m_enable_for_minimum_off_delay;

		void wait_for_minimum_offdelay()const;
	public:

		MirrorStateOff(bool _enable_for_minimum_off_delay=true);
		void init();
		MirrorState* wait_for_state()const;
	};
};

class MirrorStateManager{
	public:
	void run();
};