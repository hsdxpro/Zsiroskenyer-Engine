#include "PhysicsEngineBullet.h"

#include "..\..\Core\src\common.h"

#include "RigidEntityBullet.h"

#include "..\..\Core\src\GeometryBuilder.h"
#include <algorithm>
#include <list>

#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

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
	
	// GImpact dispatching registering, need for some special collision variances
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	
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
		if (mass == 0) {
			// Bullet bhv opt container

			int* indices = new int[d.nIndices];
			memcpy(indices, d.indices, d.indexStride * d.nIndices);

			btVector3* vertices = new btVector3[d.nVertices];
			for (size_t i = 0; i < d.nVertices; i++) {
				memcpy(vertices[i], (unsigned char*)d.vertices + i * d.vertexStride, sizeof(Vec3));
			}

			//Bullet Tároló feltöltése, általunk kigyüjtött adatokkal
			btTriangleIndexVertexArray* VBIB = new btTriangleIndexVertexArray(d.nIndices / 3,
				(int*)indices,
				3 * d.indexStride,
				d.nVertices, (btScalar*)vertices, sizeof(btVector3));

			colShape = new btBvhTriangleMeshShape(VBIB, true);
			
		} else { // Dynamic object
			colShape = new btConvexHullShape((btScalar*)d.vertices, d.nVertices, d.vertexStride);
		}

		collisionShapes[physicsGeom] = colShape;
	} else {
		colShape = collisionShapes[physicsGeom];
	}
	
	// Dynamic Physics entity, calculate local inertia
	btVector3 localInertia(0,0,0);
	if (mass != 0)
		colShape->calculateLocalInertia(mass, localInertia);
		
	// Create rigid body
	btRigidBody* body = new btRigidBody(mass, new btDefaultMotionState(), colShape, localInertia);
	physicsWorld->addRigidBody(body);
	cRigidEntityBullet* r = new cRigidEntityBullet(body);
	return r;
}

void cPhysicsEngineBullet::ShootBox(float size, const Vec3& pos, const Vec3& dir, float power) {
	btTransform asd;
	asd.setIdentity();
	asd.setOrigin(btVector3(pos.x, pos.y, pos.z));

	float mass = 1.0f;
	btMotionState* motionstate = new btDefaultMotionState(asd);
	btCollisionShape* colshape = new btBoxShape(btVector3(size, size, size));
	btVector3 localinertia(0.0f, 0.0f, 0.0f);
	btRigidBody* body = new btRigidBody(mass, motionstate, colshape, localinertia);

	//Érdekes észrevétel sajnos...
	//Csak azoknak a rigidbodykra lehet új erõt stb késztetni, amelyikeknek utoljára
	//adtuk hozzá a RigidBody ját a világhoz...

	physicsWorld->addRigidBody(body);

	body->setLinearFactor(btVector3(1, 1, 1));

	btVector3 linVel(dir.x, dir.y, dir.z);
	body->setLinearVelocity(linVel * power);

	body->setContactProcessingThreshold(1e30);

	///when using m_ccdMode, disable regular CCD
	body->setCcdMotionThreshold(2.5f);
	body->setCcdSweptSphereRadius(0.4f);

}

void cPhysicsEngineBullet::GetCollisionShapeEdges(Vec3* edges, size_t size, size_t& nEdges) {
	// Edge points
	btVector3 p1, p2;

	auto colObjArray = physicsWorld->getCollisionWorld()->getCollisionObjectArray();
	size_t nObjs = physicsWorld->getNumCollisionObjects();

	// Vec3* edges to small
	//ASSERT(size >= nObjs * sizeof(Vec3));

	size_t edgeIndex = 0;
	nEdges = 0;
	for (size_t i = 0; i < nObjs; i++) {
		btCollisionShape* colShape = colObjArray[i]->getCollisionShape();
		btTransform worldTrans = colObjArray[i]->getWorldTransform();

		// Add each edge from convex Shape to the list
		if(colShape->isConvex()) {
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
			btBvhTriangleMeshShape* concCol = (btBvhTriangleMeshShape*)colShape;

			// Get concave mesh description ...
			const IndexedMeshArray& iMa = ((btTriangleIndexVertexArray*)concCol->getMeshInterface())->getIndexedMeshArray();
			
			// For each mesh in concave object
			for (size_t j = 0; j < iMa.size(); j++) {
				const btIndexedMesh& mesh = iMa.at(j);

				nEdges += mesh.m_numTriangles * 3;

				// IB, VB
				int* indices = (int*)mesh.m_triangleIndexBase;
				btVector3* vertices = (btVector3*)mesh.m_vertexBase;

				// For each triangle
				btVector3 verticesPos[3];
				for (size_t k = 0; k < mesh.m_numTriangles; k++) {
					
					// Define three vertices in world space
					for (size_t l = 0; l < 3; l++)
						verticesPos[l] = worldTrans * vertices[indices[k * 3 + l]];

					// Edge 1
					edges[edgeIndex].x = verticesPos[0].x();
					edges[edgeIndex].y = verticesPos[0].y();
					edges[edgeIndex].z = verticesPos[0].z();
					edgeIndex++;
					
					edges[edgeIndex].x = verticesPos[1].x();
					edges[edgeIndex].y = verticesPos[1].y();
					edges[edgeIndex].z = verticesPos[1].z();
					edgeIndex++;
					
					
					// Edge 1
					edges[edgeIndex].x = verticesPos[1].x();
					edges[edgeIndex].y = verticesPos[1].y();
					edges[edgeIndex].z = verticesPos[1].z();
					edgeIndex++;
					edges[edgeIndex].x = verticesPos[2].x();
					edges[edgeIndex].y = verticesPos[2].y();
					edges[edgeIndex].z = verticesPos[2].z();
					edgeIndex++;

					// Edge 2
					edges[edgeIndex].x = verticesPos[2].x();
					edges[edgeIndex].y = verticesPos[2].y();
					edges[edgeIndex].z = verticesPos[2].z();
					edgeIndex++;
					edges[edgeIndex].x = verticesPos[0].x();
					edges[edgeIndex].y = verticesPos[0].y();
					edges[edgeIndex].z = verticesPos[0].z();
					edgeIndex++;
				}
			}
		}
	}
}