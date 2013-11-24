#include "PhysicsEngineBullet.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

#include "..\..\Core\src\GeometryBuilder.h"
#include "..\..\Core\src\common.h"
#include <algorithm>
#include <list>

#include "RigidEntityBullet.h"
#include "SoftEntityBullet.h"

// Soft body test
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

cPhysicsEngineBullet::cPhysicsEngineBullet() {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btSoftBodyRigidBodyCollisionConfiguration* collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();

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
	//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	
	physicsWorld->setGravity(btVector3(0 ,0 ,-10)); // TODO TMP GRAV

	physicsWorld->getDispatchInfo().m_useContinuous = true;


	softBodyWorldInfo.air_density = (btScalar)1.2;
	softBodyWorldInfo.water_density = 0;
	softBodyWorldInfo.water_offset = 0;
	softBodyWorldInfo.water_normal = btVector3(0, 0, 0);
	softBodyWorldInfo.m_gravity = physicsWorld->getGravity();
	softBodyWorldInfo.m_sparsesdf.Initialize();
	softBodyWorldInfo.m_dispatcher = physicsWorld->getDispatcher();
	softBodyWorldInfo.m_broadphase = physicsWorld->getBroadphase();
	softBodyWorldInfo.m_sparsesdf.Initialize();

	physicsWorld->getDispatchInfo().m_enableSPU = true;

	//Ragad�s Spagetti effektus :D
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
	// Geom path alapj�n btCollisionShape kikeres�se, ha nincs l�trehoz�s...
	auto it = collisionShapes.find(physicsGeom);
	if(it == collisionShapes.end()) {
		cGeometryBuilder::tGeometryDesc d = cGeometryBuilder::LoadGeometry(physicsGeom);

		// Static object
		if (mass == 0) {
			// Bullet bhv opt container

			int* indices = new int[d.nIndices];
			memcpy(indices, d.indices, d.indexStride * d.nIndices);

			btVector3* vertices = new btVector3[d.nVertices];
			for (size_t i = 0; i < d.nVertices; i++) {
				memcpy(vertices[i], (unsigned char*)d.vertices + i * d.vertexStride, sizeof(Vec3));
			}

			//Bullet T�rol� felt�lt�se, �ltalunk kigy�jt�tt adatokkal
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
	colShape->setMargin(0.5); // TODO TMP TEST

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

IPhysicsEntity* cPhysicsEngineBullet::CreateSoftEntity(const zsString& physicsGeom, float mass)  {
	// Read geometry, save infos
	cGeometryBuilder::tGeometryDesc d = cGeometryBuilder::LoadGeometry(physicsGeom);
	int* indices = new int[d.nIndices];
		memcpy(indices, d.indices, d.indexStride * d.nIndices);

	btVector3* vertices = new btVector3[d.nVertices];
	for (size_t i = 0; i < d.nVertices; i++)
		memcpy(vertices[i], (unsigned char*)d.vertices + i * d.vertexStride, sizeof(Vec3));

	// Create soft body from geometry
	//btSoftBody* body = btSoftBodyHelpers::CreateFromTriMesh(softBodyWorldInfo, (btScalar*)vertices, indices, d.nIndices / 3);

	btSoftBody* body = btSoftBodyHelpers::CreateEllipsoid(softBodyWorldInfo, btVector3(0, 0, 0), btVector3(6, 6, 6), 20);
	/*
	body->getCollisionShape()->setMargin(0.01);

	body->m_cfg.collisions = btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;

	// Assign material to soft body
	btSoftBody::Material* pm = body->appendMaterial();
	pm->m_kLST = 0.7;
	//pm->m_flags -= btSoftBody::fMaterial::DebugDraw;
	body->generateBendingConstraints(2, pm);
	body->m_cfg.piterations = 3;
	body->m_cfg.kDF = 0.5;
	body->scale(btVector3(1, 1, 1));
	body->setTotalMass(mass, true);
	body->randomizeConstraints();
	*/
	// Add soft body to world
	physicsWorld->addSoftBody(body);

	body->setVolumeMass(300);
	body->m_cfg.piterations = 1;
	//body->generateClusters(16);
	body->getCollisionShape()->setMargin(0.01);
	body->m_cfg.collisions = btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;
	body->m_materials[0]->m_kLST = 0.4;


	cSoftEntityBullet* r = new cSoftEntityBullet(body);
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

	//�rdekes �szrev�tel sajnos...
	//Csak azoknak a rigidbodykra lehet �j er�t stb k�sztetni, amelyikeknek utolj�ra
	//adtuk hozz� a RigidBody j�t a vil�ghoz...

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
		if (colShape->isConvex()) {
			btConvexHullShape* convCol = (btConvexHullShape*)colShape;
			size_t nEdgesOnColShape = convCol->getNumEdges();
			nEdges += nEdgesOnColShape;

			for (size_t j = 0; j < nEdgesOnColShape; j++) {
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
		}
		else if (colShape->isConcave()) {
			btBvhTriangleMeshShape* concCol = (btBvhTriangleMeshShape*)colShape;

			// Get concave mesh description ...
			const IndexedMeshArray& iMa = ((btTriangleIndexVertexArray*)concCol->getMeshInterface())->getIndexedMeshArray();

			// For each mesh in concave object
			for (int j = 0; j < iMa.size(); j++) {
				const btIndexedMesh& mesh = iMa.at(j);

				nEdges += mesh.m_numTriangles * 3;

				// IB, VB
				int* indices = (int*)mesh.m_triangleIndexBase;
				btVector3* vertices = (btVector3*)mesh.m_vertexBase;

				// For each triangle
				btVector3 verticesPos[3];
				for (int k = 0; k < mesh.m_numTriangles; k++) {

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

	btSoftBodyArray& arr = physicsWorld->getSoftBodyArray();
	for (int i = 0; i < arr.size(); i++) {
		btSoftBody* body = arr.at(i);
		btTransform worldTrans = body->getWorldTransform();

		btSoftBody::tFaceArray& fArr = body->m_faces;
		nEdges += fArr.size() * 3;
		for (int j = 0; j < fArr.size(); j++) {
			btSoftBody::Face& face = fArr.at(j);

			btVector3 p0 = worldTrans * face.m_n[0]->m_x;
			btVector3 p1 = worldTrans * face.m_n[1]->m_x;
			btVector3 p2 = worldTrans * face.m_n[2]->m_x;

			// Edge 1
			edges[edgeIndex].x = p0.x();
			edges[edgeIndex].y = p0.y();
			edges[edgeIndex].z = p0.z();
			edgeIndex++;

			edges[edgeIndex].x = p1.x();
			edges[edgeIndex].y = p1.y();
			edges[edgeIndex].z = p1.z();
			edgeIndex++;


			// Edge 1
			edges[edgeIndex].x = p1.x();
			edges[edgeIndex].y = p1.y();
			edges[edgeIndex].z = p1.z();
			edgeIndex++;
			edges[edgeIndex].x = p2.x();
			edges[edgeIndex].y = p2.y();
			edges[edgeIndex].z = p2.z();
			edgeIndex++;

			// Edge 2
			edges[edgeIndex].x = p2.x();
			edges[edgeIndex].y = p2.y();
			edges[edgeIndex].z = p2.z();
			edgeIndex++;
			edges[edgeIndex].x = p0.x();
			edges[edgeIndex].y = p0.y();
			edges[edgeIndex].z = p0.z();
			edgeIndex++;
		}
	}
}