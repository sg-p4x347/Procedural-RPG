#pragma once
using namespace std;

namespace History
{
	struct Value {
		float demand = 0.0;   // how much the value is needed (0.0 <-> 1.0, A percentage of how much of the parent needs it)
		float supply = 0.0;	  // how much of the value can be produced (0.0 <-> 1.0, A percentage of how much of the parent can produce it)
		float quantity = 0.0; // how much of the value is owned (0.0 <-> 1.0, A percentage of how much of the parent owns it)
	};
	struct Values {
		// Physiological
		Value food;
		Value water;
		Value shelter;
		// Safety
		Value safety;
		// Social/Economic/Political
		Value ethos; // credibility and power
		Value pathos; // emotionality
		Value logos; // logicality
		Value wealth; // money$$
		// Technological
		Value technology; // how efficiently resources are aquired
		// Resources
		Value wood;
		Value stone;
		Value iron;
		Value coal;
		Value steel;
		Value gold;
		Value silver;
		Value copper;
		Value tin;
	};
	enum Season {
		Spring = 0,
		Summer = 1,
		Fall = 2,
		Winter = 3
	};
	struct Date {
		Date() {
			year = 0;
			season = Spring;
		}
		Date(int y,Season s) {
			year = y;
			season = s;
		}
		int year;
		Season season;
	};
	struct Event {
		Event(string desc, Date s, Date e) {
			description = desc;
			start = s;
			end = e;
		}
		string description;
		Date start;
		Date end;
	};
	/* SOCIAL
	The lowest level of division; represents people and their basic values and needs
	*/
	class Social {
	public:
		Social(string name, int pop);
		void Update(Date date);
		string GetName();
		int GetPop();
		Values GetValues();
	private:
		string m_name;
		int m_population;
		Values m_values;
		vector<Event> m_events; // a timeline of events
		int Logistic(int time);
	};
	/* LOCAL
	The second level of division; represents towns and cities and their governance over their citizens
	*/
	class Local {
	public:
		Local(string name);
		void Update();
		string GetName();
		int GetPop();
		Values GetValues();
	private:
		string m_name;
		vector<Social> m_socials;
		Values m_values;
		vector<Event> m_events; // a timeline of events
	};
	/* STATE
	The third level of division; represents large regions that contain several cities
	*/
	class State {
	public:
		State(string name);
		void Update();
		string GetName();
		int GetPop();
		Values GetValues();
	private:
		string m_name;
		vector<Social> m_socials; // for people who do not live in the cities
		vector<Local> m_locals;
		Values m_values;
		vector<Event> m_events; // a timeline of events
	};
	/* FEDERAL
	The highest level of division; represents a single empire to govern all the state governments
	*/
	class Federal {
	public:
		Federal();
		Federal(string name);
		void Update();
		string GetName();
		int GetPop();
		Values GetValues();
	private:
		string m_name;
		vector<State> m_states;
		Values m_values;
		vector<Event> m_events; // a timeline of events
	};
	// Main regulatory class
	class Timeline {
	public:
		Timeline();
		void Update();
	private:
		vector<Event> m_events; // a timeline of events
		Date m_date;
		Federal m_federal; // the entire empire
	};
};

