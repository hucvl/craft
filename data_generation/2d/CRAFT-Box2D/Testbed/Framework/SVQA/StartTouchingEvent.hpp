//
//  StartTouchingEvent.hpp
//  Testbed
//
//  Created by Tayfun Ateş on 28.01.2020.
//

#ifndef StartTouchingEvent_hpp
#define StartTouchingEvent_hpp

#include "CausalEvent.hpp"

namespace svqa
{
    class StartTouchingEvent : public CausalEvent
    {
        public:
            typedef std::shared_ptr<StartTouchingEvent> Ptr;
        
            StartTouchingEvent(const int& step, BODY* firstObject, BODY* secondObject) : CausalEvent(step), m_pFirstObject(firstObject), m_pSecondObject(secondObject) {};
            virtual ~StartTouchingEvent() {};
        
            static Ptr create(const int& step, BODY* firstObject, BODY* secondObject)
            {
                return std::make_shared<StartTouchingEvent>(step, firstObject, secondObject);
            }
        
            virtual CausalEventType getType() const override
            {
                return StartTouching_Event;
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

#endif /* StartTouchingEvent_hpp */
