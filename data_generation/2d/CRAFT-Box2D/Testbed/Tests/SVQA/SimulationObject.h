//
//  SimulationObject.h
//  Testbed
//
//  Created by Tayfun Ateş on 9.10.2019.
//

#ifndef SimulationObject_h
#define SimulationObject_h

#include <nlohmann/json.hpp>
#include "Box2D/Common/b2Draw.h"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include "Box2D/Collision/Shapes/b2CircleShape.h"
#include "Box2D/Collision/Shapes/b2EdgeShape.h"
#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include "Box2D/Collision/Shapes/b2ChainShape.h"
#include "SimulationDefines.h"
#include <ctime>
typedef std::shared_ptr<b2Shape> ShapePtr;


#if defined(__APPLE_CC__)
#else
#define M_PI 3.14159265f
#endif

class SimulationObject
{
public:
    // TODO: Maybe put it inside an enum named SensorBit?
    static const int SENSOR_BASKET = 0x042;

    enum Shape
    {
        CUBE = 0,
        TRIANGLE = 1,
        CIRCLE = 2,
        STATIC_RAMP = 3,
        STATIC_PLATFORM = 4,
        STATIC_BASKET = 5,
        STATIC_TABLE = 6,
        STATIC_LEFT_BOUNDARY =7,
        STATIC_RIGHT_BOUNDARY = 8,
        STATIC_BOTTOM_BOUNDARY = 9,
        STATIC_BALL = 10,
    };
    
    enum Color
    {
        GRAY = 0,
        RED = 1,
        BLUE = 2,
        GREEN = 3,
        BROWN = 4,
        PURPLE = 5,
        CYAN = 6,
        YELLOW = 7,
        BLACK = 8
    };
    
    enum Size
    {
        SMALL = 0,
        LARGE = 1
    };
    
	SimulationObject(Shape sh, Color c, Size sz)
	{
		mShape = sh;
        mColor = c;
        mSize = sz;
	}
    

    static Size getRandomSize() {
        
        int i = std::rand() % 2;
        return Size(i); 
    }

    static Color getRandomColor() {
        //RandomFloatFromHardware(15.0, 18.0)
        int i = std::rand() % 8;
        return Color(i);
    }

    static Shape getRandomShape() {
        
        int i = std::rand() % 3;
        return Shape(i);
    }


    static std::string getRepresentation(Shape sh)
    {
        switch (sh)
        {
            case CUBE: return "cube";
            case TRIANGLE: return "triangle";
            case CIRCLE: return "circle";
            case STATIC_RAMP: return "ramp";
            case STATIC_TABLE: return "table";
            case STATIC_PLATFORM: return "platform";
            case STATIC_BASKET: return "basket";
            case STATIC_LEFT_BOUNDARY: return "wall_l";
            case STATIC_RIGHT_BOUNDARY: return "wall_r"; //Check if using same value is ok!
            case STATIC_BOTTOM_BOUNDARY: return "ground";
            case STATIC_BALL: return "static_ball";
            default: break;
        }
        return "";
    }
        
    static std::string getRepresentation(Color c)
    {
        switch (c)
        {
            case GRAY: return "gray";
            case RED: return "red";
            case BLUE: return "blue";
            case GREEN: return "green";
            case BROWN: return "brown";
            case PURPLE: return "purple";
            case CYAN: return "cyan";
            case YELLOW: return "yellow";
            case BLACK: return "black";
            default: break;
        }
        return "";
    }
    
    static std::string getRepresentation(Size sz)
    {
        switch (sz)
        {
            case SMALL: return "small";
            case LARGE: return "large";
            default: break;
        }
        return "";
    }
    
    b2Color getColor()
    {
        switch (mColor)
        {
            case GRAY: return b2Color(87.0f / 255.0f, 87.0f / 255.0f, 87.0f / 255.0f);
            case RED: return b2Color(173.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f);
            case BLUE: return b2Color(42.0f / 255.0f, 75.0f / 255.0f, 215.0f / 255.0f);
            case GREEN: return b2Color(29.0f / 255.0f, 105.0f / 255.0f, 20.0f / 255.0f);
            case PURPLE: return b2Color(129.0f / 255.0f, 38.0f / 255.0f, 192.0f / 255.0f);
            case BROWN: return b2Color(129.0f / 255.0f, 74.0f / 255.0f, 25.0f / 255.0f);
            case CYAN: return b2Color(41.0f / 255.0f, 208.0f / 255.0f, 208.0f / 255.0f);
            case YELLOW: return b2Color(255.0f / 255.0f, 238.0f / 255.0f, 51.0f / 255.0f);
            case BLACK: return b2Color(0, 0, 0);
            default: break;
        }
        return b2Color(0, 0, 0);
    }

   
    
    bool isStatic() const
    {
        switch (mShape)
        {
            case STATIC_RAMP:
            case STATIC_PLATFORM:
            case STATIC_BASKET:
            case STATIC_TABLE:
            case STATIC_LEFT_BOUNDARY:
            case STATIC_RIGHT_BOUNDARY:
            case STATIC_BOTTOM_BOUNDARY:
            case STATIC_BALL:
                return true;
            default: break;
        }
        return false;
    }


	ShapePtr getShape()
	{
        float length = (mSize == Size::SMALL ? 1.0f : 2.0f);
        
		switch (mShape) {
		case CUBE:
            return std::make_shared<b2PolygonShape>(getRectangle(length, length));
        case TRIANGLE:
            return std::make_shared<b2PolygonShape>(getPolygon(length + 1.5f, 3));
        case CIRCLE:
            return std::make_shared<b2PolygonShape>(getPolygon(length, 128));
        case STATIC_RAMP:
            return std::make_shared<b2PolygonShape>(getRightTriangle(3.0f, 3));
        case STATIC_TABLE:
            return std::make_shared<b2PolygonShape>(getRectangle(13.0f, 10.0f));
        case STATIC_PLATFORM:
            return std::make_shared<b2PolygonShape>(getRectangle(8.0f, 0.5f));
        case STATIC_BASKET:
            return std::shared_ptr<b2ChainShape>(getBasketShape());
        case STATIC_LEFT_BOUNDARY:
            return std::make_shared<b2PolygonShape>(getRectangle(0.20, 25, b2Vec2(-25.0f, 20.0f), 0.0f));
        case STATIC_RIGHT_BOUNDARY:
            return std::make_shared<b2PolygonShape>(getRectangle(0.20, 25, b2Vec2(25.0f, 20.0f), 0.0f));
        case STATIC_BOTTOM_BOUNDARY:
            return std::make_shared<b2PolygonShape>(getRectangle(0.20, 25, b2Vec2(0.0f, -5.0f), M_PI / 2));
        case STATIC_BALL:
            return std::make_shared<b2PolygonShape>(getPolygon(3.0f, 128));
		}
		return nullptr;
	}

	Shape mShape;
    Color mColor;
    Size mSize;
    
    float getFriction() const
    {
        return 0.10f;
    }
    
    float getDensity() const
    {
        return 50.0f;
    }
    
    float getRestitution() const
    {
        if(isStatic()) {
            return 0.2f;
        }
        return 0.2f;
    }

	static b2VisPolygonShape getCube(const float& edgeLength)
	{
		b2VisPolygonShape shape;
		shape.SetAsBox(edgeLength, edgeLength);
		return shape;
	}

	static b2VisPolygonShape getRectangle(const float& w, const float& h)
	{
		b2VisPolygonShape shape;
		shape.SetAsBox(w, h);
		return shape;
	}

	static b2VisPolygonShape getRectangle(const float& w, const float& h, b2Vec2 center, float32 angle)
	{
		b2VisPolygonShape shape;
		shape.SetAsBox(w, h, center, angle);
		return shape;
	}

	static b2Vec2 rotate(const b2Vec2& v, const float anglesInRadians)
	{
		const float s = sin(anglesInRadians);
		const float c = cos(anglesInRadians);
		b2Vec2 ret = v;
		b2Vec2 translatedPoint = ret;
		translatedPoint.x = ret.x * c - ret.y * s;
		translatedPoint.y = ret.x * s + ret.y * c;
		ret.x = translatedPoint.x;
		ret.y = translatedPoint.y;
		return ret;
	}

	static b2VisPolygonShape getPolygon(const float& edgeLength, const int& nPoints)
	{
		const float twoPi = 2.0f * M_PI;
		std::vector<b2Vec2> points(nPoints);

        int y = nPoints == 4 ? 0 : -edgeLength;
		points[0] = b2Vec2(0,y);

		b2Vec2 currentPoint = points[0];
		const float rotationAngle = twoPi / nPoints;
		for (size_t i = 1; i < points.size(); i++)
		{
			currentPoint = rotate(currentPoint, rotationAngle);
			points[i] = currentPoint;
		}
		b2VisPolygonShape shape;
		shape.Set(points.data(), nPoints);
		return shape;
	}

	static b2CircleShape getCircle(const float& radius)
	{
		b2CircleShape circle;
		circle.m_radius = radius;
		return circle;
	}

	static b2ChainShape getCurve(std::function<float(float)> function, float fromX, float toX, float step)  
	{
		b2ChainShape shape = b2ChainShape();
		std::vector<b2Vec2> vertices = std::vector<b2Vec2>();
		float x = fromX;
		while (x <= toX) 
		{
			vertices.push_back(b2Vec2(x, function(x)));
			x += step;
		}

		shape.CreateChain(vertices.data(), vertices.size());
		return shape;
	}

	static b2VisPolygonShape getRightTriangle(const float& vEdgeLength, const float& hEdgeLength)
	{
		b2VisPolygonShape shape;
		// TODO
		b2Vec2 vertex1 = b2Vec2(vEdgeLength, 0);
		b2Vec2 vertex2 = b2Vec2(0, hEdgeLength);
		b2Vec2 vertex3 = b2Vec2(0, 0);

		std::vector<b2Vec2>  vertices = std::vector<b2Vec2>();
		vertices.push_back(vertex1);
		vertices.push_back(vertex2);
		vertices.push_back(vertex3);

		shape.Set(vertices.data(), 3);
		return shape;
	}

	static b2VisPolygonShape getCarBodyIrregularPolygon()
	{
		b2VisPolygonShape chassis;
		b2Vec2 vertices[8];
		const float scale = 3.0;
		vertices[0].Set(scale * -1.5f, scale * -0.5f);
		vertices[1].Set(scale * 1.5f, scale * -0.5f);
		vertices[2].Set(scale * 1.5f, scale * 0.0f);
		vertices[3].Set(scale * 0.0f, scale * 0.9f);
		vertices[4].Set(scale * -1.15f, scale * 0.9f);
		vertices[5].Set(scale * -1.5f, scale * 0.2f);
		chassis.Set(vertices, 6);
		return chassis;
	}

    static b2ChainShape* getBasketShape()
    {
        b2ChainShape* shape = new b2ChainShape;
        std::vector<b2Vec2> vertices = { b2Vec2(-3.5f, 3.5), b2Vec2(-2.5, -3.5), b2Vec2(2.5, -3.5), b2Vec2(3.5, 3.5) };
        shape->CreateChain(vertices.data(), vertices.size());
        return shape;
    }
    
    static b2EdgeShape getBoundaryEdgeShape(b2Vec2 xs, b2Vec2 ys)
    {
        b2EdgeShape shape;
        shape.Set(xs, ys);
        return shape;
    }
};

NLOHMANN_JSON_SERIALIZE_ENUM(SimulationObject::Shape, {
	{SimulationObject::CUBE, SimulationObject::getRepresentation(SimulationObject::CUBE)},
	{SimulationObject::TRIANGLE, SimulationObject::getRepresentation(SimulationObject::TRIANGLE)},
    {SimulationObject::CIRCLE, SimulationObject::getRepresentation(SimulationObject::CIRCLE)},
    {SimulationObject::STATIC_TABLE, SimulationObject::getRepresentation(SimulationObject::STATIC_TABLE)},
    {SimulationObject::STATIC_RAMP, SimulationObject::getRepresentation(SimulationObject::STATIC_RAMP)},
    {SimulationObject::STATIC_PLATFORM, SimulationObject::getRepresentation(SimulationObject::STATIC_PLATFORM)},
    {SimulationObject::STATIC_BASKET, SimulationObject::getRepresentation(SimulationObject::STATIC_BASKET)},
    {SimulationObject::STATIC_LEFT_BOUNDARY, SimulationObject::getRepresentation(SimulationObject::STATIC_LEFT_BOUNDARY)},
    {SimulationObject::STATIC_RIGHT_BOUNDARY, SimulationObject::getRepresentation(SimulationObject::STATIC_RIGHT_BOUNDARY)},
    {SimulationObject::STATIC_BOTTOM_BOUNDARY, SimulationObject::getRepresentation(SimulationObject::STATIC_BOTTOM_BOUNDARY)},
    {SimulationObject::STATIC_BALL, SimulationObject::getRepresentation(SimulationObject::STATIC_BALL)},
	})

NLOHMANN_JSON_SERIALIZE_ENUM( SimulationObject::Color, {
    {SimulationObject::GRAY, SimulationObject::getRepresentation(SimulationObject::GRAY)},
    {SimulationObject::RED, SimulationObject::getRepresentation(SimulationObject::RED)},
    {SimulationObject::BLUE, SimulationObject::getRepresentation(SimulationObject::BLUE)},
    {SimulationObject::GREEN, SimulationObject::getRepresentation(SimulationObject::GREEN)},
    {SimulationObject::BROWN, SimulationObject::getRepresentation(SimulationObject::BROWN)},
    {SimulationObject::PURPLE, SimulationObject::getRepresentation(SimulationObject::PURPLE)},
    {SimulationObject::CYAN, SimulationObject::getRepresentation(SimulationObject::CYAN)},
    {SimulationObject::YELLOW, SimulationObject::getRepresentation(SimulationObject::YELLOW)},
    {SimulationObject::BLACK, SimulationObject::getRepresentation(SimulationObject::BLACK)}
})

NLOHMANN_JSON_SERIALIZE_ENUM( SimulationObject::Size, {
    {SimulationObject::SMALL, SimulationObject::getRepresentation(SimulationObject::SMALL)},
    {SimulationObject::LARGE, SimulationObject::getRepresentation(SimulationObject::LARGE)}
})

#endif /* SimulationObject_h */
