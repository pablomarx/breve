/*****************************************************************************
 *                                                                           *
 * The breve Simulation Environment                                          *
 * Copyright (C) 2000, 2001, 2002, 2003 Jonathan Klein                       *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *****************************************************************************/

/*! \addtogroup InternalFunctions */
/*@{*/

#include "kernel.h"
#include "world.h"
#include "multibody.h"

#define BRMULTIBODYPOINTER(p) ((slMultibody*)BRPOINTER(p))
#define BRLINKPOINTER(p) ((slLink*)BRPOINTER(p))

/*!
	\brief Sets the root of the multibody.

	void mutibodySetRoot(slMultiBody pointer, slLink pointer).

	Sets the root of the multibody to the given link.
*/

int brISetMultibodyRoot( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb = BRMULTIBODYPOINTER( &args[0] );
	slLink *root = BRLINKPOINTER( &args[1] );

	mb->setRoot( root );

	return EC_OK;
}

/*!
	\brief Creates a new multibody.

	void multibodyNew().

	Creates a new, unattached multibody.
*/

int brIMultibodyNew( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb;

	mb = new slMultibody( i->engine->world );
	mb->setCallbackData( i );

	target->set( mb );

	return EC_OK;
}

/**
	\brief Sets the intra-body CFM and ERP values for contact collisions.
*/

int brIMultibodySetERPCFM( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb = ( slMultibody* )BRPOINTER( &args[0] );

	mb->setERP( BRFLOAT( &args[1] ) );
	mb->setCFM( BRFLOAT( &args[2] ) );

	return EC_OK;
}

/*!
	\brief Returns a list of all objects in a multibody.

	void multibodyAllObjects(slMultibody pointer).

	Returns a list of all links and joints associated with a multibody.
*/

int brIMultibodyAllObjects( brEval args[], brEval *target, brInstance *i ) {
	brEval e;
	brEvalListHead *all;
	slList *l, *start;

	slMultibody *m = BRMULTIBODYPOINTER( &args[0] );

	all = new brEvalListHead();

	start = m->allCallbackData();

	for ( l = start; l; l = l->next ) {
		e.set(( brInstance * )l->data );

		if ( l->data )
			brEvalListInsert( all, 0, &e );
	}

	slListFree( start );

	target->set( all );

	return EC_OK;
}

/*!
	\brief Destroys a multibody.

	void multibodyFree(slMultibody).

	Destroys a multibody, but leaves all connected objects intact.
*/

int brIMultibodyFree( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *m = BRMULTIBODYPOINTER( &args[0] );

	i->engine->world->setUninitialized();

	delete m;

	return EC_OK;
}

/*!
	\brief Sets the location of an entire multibody.

	void multibodySetLocation(slMultibody pointer, vector).
*/

int brIMultibodySetLocation( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb = BRMULTIBODYPOINTER( &args[0] );
	slVector *v = &BRVECTOR( &args[1] );

	mb->setLocation( v );

	return EC_OK;
}

/*!
	\brief Sets the rotation of an entire multibody.

	void multibodySetLocation(slMultibody pointer, vector, double).

	Rotates about the given vector by the given double amount.  Rotation
	occurs about the centerpoint of the root object.
*/

int brIMultibodySetRotation( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb = BRMULTIBODYPOINTER( &args[0] );
	slVector *v = &BRVECTOR( &args[1] );
	double len = BRDOUBLE( &args[2] );
	double rot[3][3];

	slRotationMatrix( v, len, rot );

	mb->setRotation( rot );

	return EC_OK;
}

int brIMultibodySetRotationMatrix( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb = BRMULTIBODYPOINTER( &args[0] );

	mb->setRotation( BRMATRIX( &args[1] ) );

	return EC_OK;
}

/*!
	\brief Does a relative rotation on a multibody.

	void multibodyRotateRelative(slMultibody pointer, vector, double).

	Rotates a multibody around a given vector by a given double amount.
*/

int brIMultibodyRotateRelative( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *mb = BRMULTIBODYPOINTER( &args[0] );
	slVector *v = &BRVECTOR( &args[1] );
	double len = BRDOUBLE( &args[2] );
	double rotation[3][3];

	if ( !mb ) {
		slMessage( DEBUG_ALL, "null pointer passed to multibodyRotateRelative\n" );
		return EC_ERROR;
	}

	slRotationMatrix( v, len, rotation );

	mb->rotate( rotation );

	return EC_OK;
}

/*!
	\brief Turn self-collision handling on or off for a body.

	void multibodyHandleSelfCollisions(slWorldObject pointer body, int state).
*/

int brIMultibodySetHandleSelfCollisions( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *m = BRMULTIBODYPOINTER( &args[0] );

	m->setHandleSelfCollisions( BRINT( &args[1] ) );

	if(  i->engine->world->_initialized == 0 ) return EC_OK;

	m->initCollisionFlags( i->engine->world->_clipData );

	return EC_OK;
}

/*!
	\brief Check for self-penetrations in a body.

	int multibodyCheckSelfPenetration(slWorldObject pointer body).
*/

int brIMultibodyCheckSelfPenetration( brEval args[], brEval *target, brInstance *i ) {
	slMultibody *m = BRMULTIBODYPOINTER( &args[0] );

	if ( m ) target->set( m->checkSelfPenetration() );

	return EC_OK;
}

/*@}*/

void breveInitMultibodyFunctions( brNamespace *n ) {
	brNewBreveCall( n, "multibodyRotateRelative", brIMultibodyRotateRelative, AT_NULL, AT_POINTER, AT_VECTOR, AT_DOUBLE, 0 );
	brNewBreveCall( n, "multibodySetLocation", brIMultibodySetLocation, AT_NULL, AT_POINTER, AT_VECTOR, 0 );
	brNewBreveCall( n, "multibodySetRotation", brIMultibodySetRotation, AT_NULL, AT_POINTER, AT_VECTOR, AT_DOUBLE, 0 );
	brNewBreveCall( n, "multibodySetRotationMatrix", brIMultibodySetRotationMatrix, AT_NULL, AT_POINTER, AT_MATRIX, 0 );
	brNewBreveCall( n, "multibodyNew", brIMultibodyNew, AT_POINTER, 0 );
	brNewBreveCall( n, "multibodySetERPCFM", brIMultibodySetERPCFM, AT_NULL, AT_POINTER, AT_DOUBLE, AT_DOUBLE, 0 );
	brNewBreveCall( n, "multibodySetRoot", brISetMultibodyRoot, AT_NULL, AT_POINTER, AT_POINTER, 0 );
	brNewBreveCall( n, "multibodyAllObjects", brIMultibodyAllObjects, AT_LIST, AT_POINTER, 0 );
	brNewBreveCall( n, "multibodyFree", brIMultibodyFree, AT_NULL, AT_POINTER, 0 );
	brNewBreveCall( n, "multibodySetHandleSelfCollisions", brIMultibodySetHandleSelfCollisions, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "multibodyCheckSelfPenetration", brIMultibodyCheckSelfPenetration, AT_INT, AT_POINTER, 0 );
}
