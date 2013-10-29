#include "PhysicsEngineBullet.h"

#include "..\..\Core\src\common.h"

#include "RigidTypeBullet.h"
#include "RigidEntityBullet.h"

#include "..\..\Core\src\GeometryBuilder.h"
#include <algorithm>
#include <list>
cPhysicsEngineBullet::cPhysicsEngineBullet() {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	btVector3 worldMin(-1000, -1000, -1000);
	btVector3 worldMax( 1000,  1000,  1000);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btAxisSweep3(worldMin, worldMax);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	physicsWorld = new btSoftRigidDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	


	//GImpact Ütközés algot regizni kell a fizikai kontaktus végrehajtónál..hogy tudjon róla
	//Még lehet hogy használok majd Gimpact ütkzést...
	//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

	//drawer->setDebugMode(drawer->DBG_DrawAabb);
	//m_physicsworld->setDebugDrawer(drawer);
	
	physicsWorld->setGravity(btVector3(0 ,0 ,-10));

	physicsWorld->getDispatchInfo().m_useContinuous = true;
	//Ragadós Spagetti effektus :D
	//m_physicsworld->getSolverInfo().m_splitImpulse=true;
	//m_physicsworld->getSolverInfo().m_numIterations = 20;
}

void cPhysicsEngineBullet::Release() {
		collisionShapes.clear();

	int i;
	for (i = physicsWorld->getNumCollisionObjects()-1; i>=0 ;i--) {
		btCollisionObject* obj = physicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();

		physicsWorld->removeCollisionObject(obj);
		SAFE_DELETE(obj);
	}

	SAFE_DELETE(physicsWorld);

	for(auto it = physicsTypes.begin() ; it != physicsTypes.end(); it++)
		SAFE_DELETE(*it);
	physicsTypes.clear();
	
	for(auto it = entities.begin() ; it != entities.end(); it++)
		SAFE_DELETE(*it);
	entities.clear();
}

void cPhysicsEngineBullet::SimulateWorld(float deltaT) {
	physicsWorld->stepSimulation(deltaT, 1);
}

IPhysicsEntity* cPhysicsEngineBullet::AddRigidEntity(const IPhysicsType* type, const Vec3& position) {
	
	cRigidTypeBullet* rigidType = (cRigidTypeBullet*)type;

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(position.x, position.y, position.z));

	// /Rigid things -> motionState
	// RigidType things mass, collisionShape, localInertia
	btRigidBody* body = new btRigidBody(rigidType->GetMass(), new btDefaultMotionState(trans), rigidType->GetCollisionShape(), rigidType->GetLocalInertia());
	physicsWorld->addRigidBody(body);
	cRigidEntityBullet * r = new cRigidEntityBullet(body);
	entities.push_back(r);
	return r;
}

IPhysicsType* cPhysicsEngineBullet::GetRigidType(const zsString& geomPath, float mass) {

	cGeometryBuilder b;
	cGeometryBuilder::tGeometryDesc d = b.LoadGeometry(geomPath);

	IPhysicsType* type;
	// Geom doesn't exists, create it
	if(! IsGeometryExists(geomPath)) {
		// Collision shape
		btCollisionShape* colShape = new btConvexHullShape((btScalar*)d.vertices, d.nVertices, d.vertexStride);

		// New rigid Type
		type = new cRigidTypeBullet(colShape, mass);
		physicsTypes.push_back(type);
		return type;
	} else /*Geom exists*/ {
		// Search for equal massed type
		type = new cRigidTypeBullet(collisionShapes[geomPath], mass);
		auto it = find(physicsTypes.begin(), physicsTypes.end(), type);

		// Doesn't exists that mass
		if(it == physicsTypes.end())
			physicsTypes.push_back(type);
		else {
			delete type;
			type = *it;
		}
	}
	return type;
}

btRigidBody* cPhysicsEngineBullet::ShootBox(const Vec3& camPos,const Vec3& destination)
{
	if (physicsWorld) {
		btTransform asd;
		asd.setIdentity();
		asd.setOrigin(btVector3(camPos.x, camPos.y, camPos.z));

		float mass = 1.0f;
		btMotionState* motionstate = new btDefaultMotionState(asd);
		btCollisionShape* colshape = new btBoxShape(btVector3(10.0f,10.0f,10.0f));
		btVector3 localinertia(0.0f,0.0f,0.0f);
		btRigidBody* body = new btRigidBody(mass,motionstate,colshape,localinertia);
		
		//Érdekes észrevétel sajnos...
		//Csak azoknak a rigidbodykra lehet új erõt stb késztetni, amelyikeknek utoljára
		//adtuk hozzá a RigidBody ját a világhoz...

		physicsWorld->addRigidBody(body);

		body->setLinearFactor(btVector3(1,1,1));
	
		btVector3 linVel(destination[0]-camPos[0],destination[1]-camPos[1],destination[2]-camPos[2]);

		body->setLinearVelocity(linVel);
		body->setAngularVelocity(btVector3(0,0,0));
		body->setContactProcessingThreshold(1e30);

		///when using m_ccdMode, disable regular CCD
		body->setCcdMotionThreshold(2.5f);
		body->setCcdSweptSphereRadius(0.4f);
		
		return body;
	}
	return NULL;
}

void cPhysicsEngineBullet::GetCollisionShapeEdges(Vec3* edges, size_t size, size_t& nEdges) {
	// Edge points
	btVector3 p1, p2;
	Vec3 fP1, fP2;

	auto colObjArray = physicsWorld->getCollisionWorld()->getCollisionObjectArray();
	size_t nObjs = physicsWorld->getNumCollisionObjects();

	// Vec3* edges to small
	ZSASSERT(size >= nObjs * sizeof(Vec3));

	size_t edgeIndex = 0;
	nEdges = 0;
	for(size_t i = 0; i < nObjs; i++) {
		btCollisionShape* colShape = colObjArray[i]->getCollisionShape();
		btTransform worldTrans = colObjArray[i]->getWorldTransform();

		// Add each edge from convex Shape to the list
		if(colObjArray[i]->getCollisionShape()->isConvex()) {
			btConvexHullShape* convCol = (btConvexHullShape*)colShape;
			size_t nEdgesOnColShape = convCol->getNumEdges();
			nEdges += nEdgesOnColShape;
			for(size_t j = 0; j < nEdges; j++) {
				convCol->getEdge(j, p1, p2);

				// Transform From local space to World space
				p1 = worldTrans * p1;
				p2 = worldTrans * p2;

				// Save values
					edges[edgeIndex].x = p1.x();
					edges[edgeIndex].y = p1.y();
					edges[edgeIndex].z = p1.z();
				edgeIndex++;
					edges[edgeIndex].x = p2.x();
					edges[edgeIndex].y = p2.y();
					edges[edgeIndex].z = p2.z();
				edgeIndex++;
			}
			
		} else {
			//@TODO CONCAVE CollisionObject extraction to edge list...
		}
	}
}

bool cPhysicsEngineBullet::IsGeometryExists(const zsString& geomPath) {
	return collisionShapes.find(geomPath) != collisionShapes.end();
}