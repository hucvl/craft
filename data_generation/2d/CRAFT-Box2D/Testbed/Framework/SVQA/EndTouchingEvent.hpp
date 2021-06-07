//
//  EndTouchingEvent.hpp
//  Testbed
//
//  Created by Tayfun Ateş on 28.01.2020.
//

#ifndef EndTouchingEvent_hpp
#define EndTouchingEvent_hpp

#include "CausalEvent.hpp"

namespace svqa
{
    class EndTouchingEvent : public CausalEvent
    {
        public:
            typedef std::shared_ptr<EndTouchingEvent> Ptr;
        
            EndTouchingEvent(const int& step, BODY* firstObject, BODY* secondObject) : CausalEvent(step), m_pFirstObject(firstObject), m_pSecondObject(secondObject) {};
            virtual ~EndTouchingEvent() {};
        
            static Ptr create(const int& step, BODY* firstObject, BODY* secondObject)
            {
                return std::make_shared<EndTouchingEvent>(step, firstObject, secondObject);
            }
        
            virtual CausalEventType getType() const override
            {
                return EndTouching_Event;
            }
        
            virtual std::vector<BODY*> getObjects() const override
            {
                std::vector<BODY*> ret{ m_pFirstObject, m_pSecondObject };
                return ret;
            }
        
        private:
            BODY*   m_pFirstObject;
            BODY*   m_pSecondObject;
    };
}

#endif /* EndTouchingEvent_hpp */
