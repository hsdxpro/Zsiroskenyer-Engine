#include "PhysicsEngineBullet.h"

#include "..\..\Core\src\common.h"

#include "RigidEntityBullet.h"

#include "..\..\Core\src\GeometryBuilder.h"
#include <algorithm>
#include <list>
cPhysicsEngineBullet::cPhysicsEngineBullet() {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	btVector3 worldMin(-10000, -10000, -10000);
	btVector3 worldMax( 10000,  10000,  10000);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btAxisSweep3(worldMin, worldMax);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	physicsWorld = new btSoftRigidDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	
	//GImpact Ütközés algot regizni kell a fizikai kontaktus végrehajtónál..hogy tudjon róla
	//Még lehet hogy használok majd Gimpact ütkzést...
	//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	
	physicsWorld->setGravity(btVector3(0 ,0 ,-200.0f));

	physicsWorld->getDispatchInfo().m_useContinuous = true;

	//Ragadós Spagetti effektus :D
	//m_physicsworld->getSolverInfo().m_splitImpulse=true;
	//m_physicsworld->getSolverInfo().m_numIterations = 20;
}

cPhysicsEngineBullet::~cPhysicsEngineBullet() {
	int i;
	for (i = physicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = physicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();

		physicsWorld->removeCollisionObject(obj);
		SAFE_DELETE(obj);
	}

	SAFE_DELETE(physicsWorld);

	collisionShapes.clear();
}

void cPhysicsEngineBullet::Release() {
	delete this;
}

void cPhysicsEngineBullet::SimulateWorld(float deltaT) {
	physicsWorld->stepSimulation(deltaT);
}

IPhysicsEntity* cPhysicsEngineBullet::CreateRigidEntity(const zsString& physicsGeom, float mass) {
	
	btCollisionShape* colShape;
	// Geom path alapján btCollisionShape kikeresése, ha nincs létrehozás...
	auto it = collisionShapes.find(physicsGeom);
	if(it == collisionShapes.end()) {
		cGeometryBuilder b;
		cGeometryBuilder::tGeometryDesc d = b.LoadGeometry(physicsGeom);

				// Static object
		//if (mass == 0) {
			// TODO GÁZ VAN
			// Bullet bhv opt container
			//btTriangleIndexVertexArray* VBIB = new btTriangleIndexVertexArray(d.nIndices / 3, (int*)d.indices, d.indexStride, d.nVertices, (btScalar*)d.vertices, d.vertexStride);
																				
			//colShape = new btBvhTriangleMeshShape(VBIB, true);
		//} else { // Dynamic object
			colShape = new btConvexHullShape((btScalar*)d.vertices, d.nVertices, d.vertexStride);
		//}

		
		collisionShapes[physicsGeom] = colShape;
	} else {
		colShape = collisionShapes[physicsGeom];
	}
	
	// Dynamic Physics entity, calculate local inertia
	btVector3 localInertia;
	if (mass != 0)
		colShape->calculateLocalInertia(mass, localInertia);
		

	// Create rigid body
	btRigidBody* body = new btRigidBody(mass, new btDefaultMotionState(), colShape, localInertia);
	physicsWorld->addRigidBody(body);
	cRigidEntityBullet* r = new cRigidEntityBullet(body);
	return r;
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
	ASSERT(size >= nObjs * sizeof(Vec3));

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

			for(size_t j = 0; j < nEdgesOnColShape; j++) {
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