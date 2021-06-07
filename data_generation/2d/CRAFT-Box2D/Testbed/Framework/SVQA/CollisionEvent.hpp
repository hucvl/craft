//
//  CollisionEvent.hpp
//  Testbed
//
//  Created by Tayfun Ateş on 28.01.2020.
//

#ifndef CollisionEvent_hpp
#define CollisionEvent_hpp

#include "CausalEvent.hpp"

namespace svqa
{
    class CollisionEvent : public CausalEvent
    {
        public:
            typedef std::shared_ptr<CollisionEvent> Ptr;
        
            CollisionEvent(const int& step, BODY* firstObject, BODY* secondObject) : CausalEvent(step), m_pFirstObject(firstObject), m_pSecondObject(secondObject) {};
            virtual ~CollisionEvent() {};
        
            static Ptr create(const int& step, BODY* firstObject, BODY* secondObject)
            {
                return std::make_shared<CollisionEvent>(step, firstObject, secondObject);
            }
        
            virtual CausalEventType getType() const override
            {
                return Collision_Event;
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

#endif /* CollisionEvent_hpp */
