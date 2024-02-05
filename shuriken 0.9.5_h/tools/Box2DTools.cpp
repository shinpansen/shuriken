#include "Box2DTools.hpp"

static const float SCALE = 30.f;
static const uint16 GROUND_ENTITY = 0x0001;
static const uint16 PLATFORM_ENTITY = 0x0002;
static const uint16 HERO_ENTITY = 0x0004;
static const uint16 ENEMY_ENTITY = 0x0008;
static const uint16 SHURIKEN_ENTITY = 0x0016;
static const uint16 PROJECTILE_ENTITY = 0x0032;
static const uint16 GLASS_ENTITY = 0x0064;
static const uint16 FIRE_ENTITY = 0x0128;
static const uint16 SPEEDWALK_ENTITY = 0x0256;

static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

using namespace std;
using namespace sf;

Box2DTools::Box2DTools()
{
}

b2Body* Box2DTools::CreateBox(b2World* world, Vector2i pos, b2Shape* shape, float friction, float density, float angle, int category)
{
    b2BodyDef* bodyDef = new b2BodyDef;
    bodyDef->position = b2Vec2(pos.x/SCALE, pos.y/SCALE);
    bodyDef->type = GetType(category);//category == 1 || category >= 7 ? b2_staticBody : category == 2 ? b2_kinematicBody : b2_dynamicBody;
    bodyDef->angle = angle*b2_pi/180;
    b2Body* body = world->CreateBody(bodyDef);

    b2FixtureDef* fixtureDef = new b2FixtureDef;
    fixtureDef->density = density;
    fixtureDef->friction = friction;
    fixtureDef->shape = shape;

    //CategoryBits : who am I ?    MaskBits : who can I collide with ?
    fixtureDef->filter.categoryBits = Entity(category);
    if(category == 1)
        fixtureDef->filter.maskBits = GROUND_ENTITY|ENEMY_ENTITY|SHURIKEN_ENTITY|PROJECTILE_ENTITY;
    else if(category == 2)
        fixtureDef->filter.maskBits = HERO_ENTITY|ENEMY_ENTITY;
    else if(category == 3)
        fixtureDef->filter.maskBits = GROUND_ENTITY|PLATFORM_ENTITY|PROJECTILE_ENTITY|SPEEDWALK_ENTITY;
    else if(category == 4)
        fixtureDef->filter.maskBits = GROUND_ENTITY|ENEMY_ENTITY|SHURIKEN_ENTITY|GLASS_ENTITY|SPEEDWALK_ENTITY;
    else if(category == 5)
        fixtureDef->filter.maskBits = GROUND_ENTITY|ENEMY_ENTITY;
    else if(category == 6)
        fixtureDef->filter.maskBits = GROUND_ENTITY|HERO_ENTITY|PROJECTILE_ENTITY;
    else if(category == 7)
        fixtureDef->filter.maskBits = ENEMY_ENTITY;
    else if(category == 8)
        fixtureDef->filter.maskBits = GROUND_ENTITY;
    else if(category == 9)
        fixtureDef->filter.maskBits = HERO_ENTITY|ENEMY_ENTITY|SHURIKEN_ENTITY|PROJECTILE_ENTITY;
    else if(category == 10)
        fixtureDef->filter.maskBits = GROUND_ENTITY|PLATFORM_ENTITY|ENEMY_ENTITY|SHURIKEN_ENTITY|GLASS_ENTITY|SPEEDWALK_ENTITY;
    else if(category == 11)
        fixtureDef->filter.maskBits = HERO_ENTITY;
    body->CreateFixture(fixtureDef);

    return body;
}

b2Body* Box2DTools::CreateRectangleBox(b2World* world, Vector2i pos, int width, int height, float friction, float density, float angle, int category)
{
    b2PolygonShape* shape = new b2PolygonShape;
    shape->SetAsBox((width/2)/SCALE, (height/2)/SCALE);
    b2Body* body = CreateBox(world, pos, shape, friction, density, angle, category);
    return body;
}

b2Body* Box2DTools::CreateCircleBox(b2World* world, Vector2i pos, int radius, float friction, float density, int category)
{
    b2CircleShape* shape = new b2CircleShape;
    shape->m_radius = ((radius/2)/SCALE);
    b2Body* body = CreateBox(world, pos, shape, friction, density, 0, category);
    return body;
}

b2Body* Box2DTools::CreatePolygonBox(b2World* world, Vector2i pos, vector<Vector2f> vertices, float friction, float density, int category)
{
    b2Vec2 b2Vertices[vertices.size()];
    for(int i=0 ; i<vertices.size() ; i++)
        b2Vertices[i].Set(vertices[i].x/SCALE, vertices[i].y/SCALE);
    b2PolygonShape* shape = new b2PolygonShape;
    shape->Set(b2Vertices, vertices.size());
    b2Body* body = CreateBox(world, pos, shape, friction, density, 0, category);
    return body;
}

bool Box2DTools::TestCollisionPoint(b2World* world, b2Vec2 point, bool all, int* collisionType, b2Body** body)
{
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        if(!BodyIterator->IsActive()) { continue; }
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Filter filter = fixture->GetFilterData();
            double height = GetAABBSize(fixture).y;
            double edge = fixture->GetBody()->GetPosition().y - (height/2);
            *collisionType = getCollisionTypeNum(fixture->GetFilterData().categoryBits);

            if(body != NULL)
                *body = fixture->GetBody();

            if(!all && (filter.categoryBits == GROUND_ENTITY || filter.categoryBits == PLATFORM_ENTITY) &&
               fixture->TestPoint(point))
                return true;
            else if(all && (filter.categoryBits == GROUND_ENTITY || filter.categoryBits == PLATFORM_ENTITY ||
                            filter.categoryBits == ENEMY_ENTITY || filter.categoryBits == SPEEDWALK_ENTITY) &&
                    fixture->TestPoint(point))
                return true;
            else if(all && (filter.categoryBits == PLATFORM_ENTITY || filter.categoryBits == ENEMY_ENTITY) &&
                    fixture->TestPoint(point) && fabs(point.y - edge) < 0.05)
                return true;
        }
    }
    return false;
}

int Box2DTools::TestHookPoint(b2World* world, b2Vec2 point, Vector2f* collisionPos, bool reversed)
{
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Filter filter = fixture->GetFilterData();
            if(filter.categoryBits == GROUND_ENTITY)
            {
                Vector2f fixtureSize = GetAABBSize(fixture);
                b2Vec2 pos = fixture->GetBody()->GetPosition();
                Vector2f edgeTopLeft = Vector2f(pos.x-(fixtureSize.x/2), pos.y-(fixtureSize.y/2));
                Vector2f edgeTopRight = Vector2f(pos.x+(fixtureSize.x/2), pos.y-(fixtureSize.y/2));
                if(fabs(point.x - edgeTopLeft.x) < 0.2 && fabs(point.y - edgeTopLeft.y) < 0.2 &&
                   point.y <= edgeTopLeft.y && !reversed)
                {
                    *collisionPos = edgeTopLeft;
                    return -1;
                }

                else if(fabs(point.x - edgeTopRight.x) < 0.2 && fabs(point.y - edgeTopRight.y) < 0.2 &&
                        point.y <= edgeTopRight.y && reversed)
                {
                    *collisionPos = edgeTopRight;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void Box2DTools::ManageBodyLoop(b2World* world)
{
    b2Filter filter;
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        if(BodyIterator == NULL)
            continue;
        filter = BodyIterator->GetFixtureList()[0].GetFilterData();
        if(filter.categoryBits == HERO_ENTITY || filter.categoryBits == ENEMY_ENTITY ||
           filter.categoryBits == SHURIKEN_ENTITY || filter.categoryBits == PROJECTILE_ENTITY)
        {
            if(BodyIterator->GetPosition().x*SCALE > NATIVE_WIDTH)
                BodyIterator->SetTransform(b2Vec2(0/SCALE,BodyIterator->GetPosition().y), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().x*SCALE < 0)
                BodyIterator->SetTransform(b2Vec2(NATIVE_WIDTH/SCALE,BodyIterator->GetPosition().y), BodyIterator->GetAngle());
            /*else if(BodyIterator->GetPosition().y*SCALE > 576)
                BodyIterator->SetTransform(b2Vec2(BodyIterator->GetPosition().x, 0/SCALE), BodyIterator->GetAngle());
            else if(BodyIterator->GetPosition().y*SCALE < 0)
                BodyIterator->SetTransform(b2Vec2(BodyIterator->GetPosition().x, 576/SCALE), BodyIterator->GetAngle());*/
        }
    }
}

void Box2DTools::DrawCollisionMask(b2World* world, RenderWindow* window)
{
    //Drawing hitboxes
    b2Transform defaultTransform(b2Vec2(0,0), b2Rot(0));
    for (b2Body* BodyIterator = world->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
    {
        //if(BodyIterator->GetType() == b2_dynamicBody) { continue; }
        if(BodyIterator == NULL)
            continue;

        bool isACircle = false;
        b2AABB aabb;
        b2CircleShape circleShapeTest;
        vector<Vector2f> vertices;
        for (b2Fixture* fixture = BodyIterator->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            fixture->GetShape()->ComputeAABB(&aabb, defaultTransform, -1);
            isACircle = fixture->GetShape()->GetType() == circleShapeTest.GetType();
            if(!isACircle)
            {
                b2PolygonShape *poly = (b2PolygonShape*)fixture->GetShape();
                for(int i=0; i<poly->GetVertexCount() ; i++)
                    vertices.push_back(Vector2f(poly->GetVertex(i).x*SCALE, poly->GetVertex(i).y*SCALE));
            }
        }

        float width = (aabb.upperBound.x*SCALE) - (aabb.lowerBound.x*SCALE) - 1;
        float height = (aabb.upperBound.y*SCALE) - (aabb.lowerBound.y*SCALE) - 1;

        //Polygon box
        ConvexShape polygon;
        polygon.setPointCount(vertices.size());
        polygon.setFillColor(Color(255,255,255,50));
        polygon.setOutlineThickness(1);
        for(int i=0 ; i<vertices.size() ; i++)
            polygon.setPoint(i, vertices[i]);
        polygon.setPosition(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y);

        //Circle box
        CircleShape circle(width/2);
        circle.setFillColor(Color(255,255,255,50));
        circle.setOutlineThickness(1);
        circle.setOrigin(width/2, width/2);
        circle.setPosition(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y);

        //Color
        if (BodyIterator->GetFixtureList()[0].GetFilterData().categoryBits == HERO_ENTITY)
            polygon.setOutlineColor(Color(255,150,150));
        else if(BodyIterator->GetFixtureList()[0].GetFilterData().categoryBits == GROUND_ENTITY)
            polygon.setOutlineColor(Color(150,255,150));
        else
            polygon.setOutlineColor(Color(150,150,255));

        //Drawing
        if(isACircle)
            window->draw(circle);
        else
            window->draw(polygon);
    }
}

uint16 Box2DTools::Entity(int i)
{
    switch(i)
    {
        case 1:
            return GROUND_ENTITY;
        case 2:
            return PLATFORM_ENTITY;
        case 3:
            return HERO_ENTITY;
        case 4:
            return ENEMY_ENTITY;
        case 5:
            return SHURIKEN_ENTITY;
        case 6:
            return PROJECTILE_ENTITY;
        case 7:
            return GLASS_ENTITY;
        case 8:
            return FIRE_ENTITY;
        case 9:
            return SPEEDWALK_ENTITY;
        case 10:
            return ENEMY_ENTITY; //Clown Enemy - can jump on platforms
        case 11:
            return PLATFORM_ENTITY;
        default:
            return GROUND_ENTITY;
    }
}

b2BodyType Box2DTools::GetType(int i)
{
    switch(i)
    {
        case 1: case 7: case 8: case 9:
            return b2_staticBody;
        case 2: case 11:
            return b2_kinematicBody;
        case 3: case 4: case 5: case 6: case 10:
            return b2_dynamicBody;
        default:
            return b2_staticBody;
    }
}

Vector2f Box2DTools::GetAABBSize(b2Fixture* fixture)
{
    b2AABB aabb = fixture->GetAABB(0);
    fixture->GetShape()->ComputeAABB(&aabb, fixture->GetBody()->GetTransform(), 0);
    double width = fabs(aabb.upperBound.x - aabb.lowerBound.x);
    double height = fabs(aabb.upperBound.y - aabb.lowerBound.y);
    return Vector2f(width, height);
}

int Box2DTools::getCollisionTypeNum(uint16 categoryBit)
{
    if(categoryBit == GROUND_ENTITY)
        return 1;
    else if(categoryBit == PLATFORM_ENTITY)
        return 2;
    else if(categoryBit == HERO_ENTITY)
        return 3;
    else if(categoryBit == ENEMY_ENTITY)
        return 4;
    else if(categoryBit == SHURIKEN_ENTITY)
        return 5;
    else if(categoryBit == PROJECTILE_ENTITY)
        return 6;
    else if(categoryBit == GLASS_ENTITY)
        return 7;
    else if(categoryBit == FIRE_ENTITY)
        return 8;
    else if(categoryBit == SPEEDWALK_ENTITY)
        return 9;
    else
        return -1;
}
