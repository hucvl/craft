//
//  CausalGraph.hpp
//  Testbed
//
//  Created by Tayfun Ateş on 28.01.2020.
//

#ifndef CausalGraph_hpp
#define CausalGraph_hpp

#include "CausalEvents.h"
#include <map>
#include <queue>
#include <iostream>

namespace svqa
{
    class CausalGraph
    {
        public:
            typedef std::shared_ptr<CausalGraph> Ptr;
        
            CausalGraph() {
            };
            virtual ~CausalGraph() {};
        
            static Ptr create()
            {
                return std::make_shared<CausalGraph>();
            }
        
            //Adds a new event to causal graph
            void addEvent(const CausalEvent::Ptr& event);
        
            //Gets json object from causal graph
            json toJSON() const;
        
            template<typename T> void printEventQueue(T& q) {
                while(!q.empty()) {
                    std::cout << q.top()->getStepCount() << " " << q.top()->getTypeStr() << std::endl;;
                    q.pop();
                }
                std::cout << '\n';
            }
        
        private:
            StartEvent::Ptr                                                                                 m_pStartEvent;      //Root of the graph
            EndEvent::Ptr                                                                                   m_pEndEvent;        //Single leaf of the graph
            std::map<BODY*, std::vector<CausalEvent::Ptr> >                                                 m_ObjectEvents;     //Map of objects an their events
  
            std::priority_queue<CausalEvent::Ptr, std::vector<CausalEvent::Ptr>, CausalEvent::Order>        m_EventQueue;
        
            //adds new events to the causal graph returning new root of the graph
            void addEventsToCausalGraph(CausalEvent::Ptr root, CausalEvent::Ptr newEvent);
        
            //get latest event of an object before any specific time step
            CausalEvent::Ptr getLatestEventBeforeTimeStep(BODY* object, int step);
        
            //get latest event of an object
            CausalEvent::Ptr getLatestEvent(BODY* object);
        
            //Constructs causal graph from objects to events mapping
            void constructCausalGraph();
    };
}

#endif /* CausalGraph_hpp */
