#ifndef ContainerEndUpEvent_hpp
#define ContainerEndUpEvent_hpp

#include "CausalEvent.hpp"

namespace svqa
{
    class ContainerEndUpEvent : public CausalEvent
    {
    public:
        typedef std::shared_ptr<ContainerEndUpEvent> Ptr;

        ContainerEndUpEvent(const int& step, BODY* firstObject, BODY* secondObject) : CausalEvent(step), m_pFirstObject(firstObject), m_pSecondObject(secondObject) {};
        virtual ~ContainerEndUpEvent() {};

        static Ptr create(const int& step, BODY* firstObject, BODY* secondObject)
        {
            return std::make_shared<ContainerEndUpEvent>(step, firstObject, secondObject);
        }

        virtual CausalEventType getType() const override
        {
            return ContainerEndUp_Event;
        }

        virtual std::vector<BODY*> getObjects() const override
        {
            std::vector<BODY*> ret{ m_pFirstObject, m_pSecondObject };
            return ret;
        }

    private:
        BODY* m_pFirstObject;
        BODY* m_pSecondObject;
    };
}

#endif /* ContainerEndUpEvent_hpp */
