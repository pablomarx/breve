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

#include "kernel.h"
#include "world.h"
#include "gldraw.h"
#include "image.h"
#include "sensor.h"
#define BRWORLDOBJECTPOINTER(p)	((slWorldObject*)BRPOINTER(p))

/*! \addtogroup InternalFunctions */
/*@{*/

/*!
	\brief Loads a GIS data file.
*/

int brIWorldLoadTigerFile( brEval args[], brEval *target, brInstance *i ) {

	target->set( slWorldLoadTigerFile( i->engine->world, BRSTRING( &args[0] ), ( slTerrain* )BRPOINTER( &args[1] ) ) );

	return EC_OK;
}

/*!
	\brief Enables or disables fast-physics.

	setStepFast(int).
*/

int brISetStepFast( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setPhysicsMode( BRINT( &args[0] ) );
	return EC_OK;
}

/*!
	\brief Sets the iterations parameter for quickstep.

	setStepFastIterations(int).
*/

int brISetStepFastIterations( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setQuickstepIterations( BRINT( &args[0] ) );
	return EC_OK;
}

int brISetAutoDisableFlag( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setAutoDisableFlag( BRINT( &args[0] ) );
	return EC_OK;
}

/*!
	\brief Enables or disables drawing of every frame.

	setDrawEveryFrame(int).

	If enabled, every frame is drawn, even if it slows down the simulation.
	When disabled, frame may be dropped in order to allow the simulation to
	run faster.
*/

int brISetDrawEveryFrame( brEval args[], brEval *target, brInstance *i ) {
	i->engine->drawEveryFrame = BRINT( &args[0] );

	return EC_OK;
}

/*!
	\brief Sets the random seed for the simulation.

	void randomSeed(int).
*/

int brIRandomSeed( brEval args[], brEval *target, brInstance *i ) {
	srandom( BRINT( &args[0] ) );
	dRandSetSeed( BRINT( &args[0] ) );

	gsl_rng_set( i->engine->RNG, BRINT( &args[0] ) );

	return EC_OK;
}

/*!
	\brief Reads a random seed from /dev/random, if possible.

	int randomSeedFromDevRandom().

	Returns -1 if the seed could not be set from /dev/random.
*/

int brIRandomSeedFromDevRandom( brEval args[], brEval *target, brInstance *i ) {
	FILE *f;
	unsigned int seed = 0;

	f = fopen( "/dev/random", "r" );

	if ( !f || !fread( &seed, sizeof seed, 1, f ) || !seed ) {
		if ( f ) fclose( f );

		if (( f = fopen( "/dev/urandom", "r" ) ) )
			fread( &seed, sizeof seed, 1, f );
	}

	if ( f ) fclose( f );

	if ( seed ) {

		target->set( 0 );

		slMessage( DEBUG_ALL, "read seed %u from random device\n", seed );

		srandom( seed );

		dRandSetSeed( seed );

		gsl_rng_set( i->engine->RNG, seed );
	} else target->set( -1 );

	return EC_OK;
}

/*!
	\brief Returns the current simulation time.

	double getTime().
*/

int brIGetTime( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->world->getAge( ) );

	return EC_OK;
}

/*!
	\brief Loads a texture from an image file [deprecated].

	USE OF THIS FUNCTION IS DEPRECATED.  The image loading methods from
	breveFunctionsImage.c are now used instead.
*/

int brILoadTexture( brEval args[], brEval *target, brInstance *i ) {
	unsigned char *pixels = NULL;
	char *path;
	int w, h, c;
	int useAlpha = BRINT( &args[1] );

	path = brFindFile( i->engine, BRSTRING( &args[0] ), NULL );

	if ( !path ) {
		slMessage( DEBUG_ALL, "Cannot locate image file for \"%s\"\n", BRSTRING( &args[0] ) );

		target->set( -1 );

		return EC_OK;
	}

	pixels = slReadImage( path, &w, &h, &c, useAlpha );

	slFree( path );

	if ( !pixels ) {
		slMessage( DEBUG_ALL, "Unrecognized image format in file \"%s\"\n", BRSTRING( &args[0] ) );

		target->set( -1 );

		return EC_OK;
	}

	target->set(( int )slTextureNew( i->engine->camera ) );

	slUpdateTexture( i->engine->camera, BRINT( target ), pixels, w, h, GL_RGBA );

	slFree( pixels );

	return EC_OK;
}

/*!
	\brief Steps the world simulation forward.

	void worldStep(double time, double integrationStep).

	The time parameter is the total time to step forward.  The integrationStep
	is a smaller value which says how fast the integrator should step forward.
*/

int brIWorldStep( brEval args[], brEval *target, brInstance *i ) {
	double totalTime = BRDOUBLE( &args[0] );
	double stepSize = BRDOUBLE( &args[1] );
	int error;

	i->engine->iterationStepSize = totalTime;

	target->set( i->engine->world->runWorld( totalTime, stepSize, &error ) );

	if ( error ) {
		brEvalError( i->engine, EE_SIMULATION, "Error in world simulation" );
		return EC_ERROR;
	}

	return EC_OK;
}

/*!
	\brief Updates the "neighbors" for neighbor-detecting objects.

	void updateNeighbors().
*/

int brIUpdateNeighbors( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->updateNeighbors();

	return EC_OK;
}

/*!
	\brief Sets the neighborhood size for an object.

	setNeighborhoodSize(slWorld

	The neighborhood size specifies how far an object looks when collecting
	neighbor information.
*/

int brISetNeighborhoodSize( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *wo = BRWORLDOBJECTPOINTER( &args[0] );
	double size = BRDOUBLE( &args[1] );

	wo->setNeighborhoodSize( size );

	return EC_OK;
}

/*!
	\brief Set the collision properties (mu, e, eT) of a stationary object.

	void setCollisionProperties(slWorldObject pointer stationary, double e, double eT, double mu).
*/

int brISetCollisionProperties( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );

	double e = BRDOUBLE( &args[1] );
	double eT = BRDOUBLE( &args[2] );
	double mu = BRDOUBLE( &args[3] );

	o->setCollisionE( e );
	o->setCollisionET( eT );
	o->setCollisionMU( mu );

	return EC_OK;
}

/*!
	\brief Gets an object's neighbors.

	list getNeighbors(slWorldObject pointer).
*/

int brIGetNeighbors( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *wo = BRWORLDOBJECTPOINTER( &args[0] );
	std::vector< void* >::iterator wi;
	brEval eval;

	target->set( new brEvalListHead() );

	std::vector< void* > &neighbors = wo->getNeighborData();

	for ( wi = neighbors.begin(); wi != neighbors.end(); wi++ ) {
		// grab the neighbor instances from the userData of the neighbors

		eval.set(( brInstance* )( *wi ) );

		if ( BRINSTANCE( &eval ) && BRINSTANCE( &eval )->status == AS_ACTIVE )
			brEvalListInsert( BRLIST( target ), 0, &eval );
	}

	return EC_OK;
}

/*!
	\brief Returns the light exposure of a single object in the world, if light
	exposure detection is in use.

	int worldObjectGetLightExposure(slWorldObject pointer).
*/

int brIWorldObjectGetLightExposure( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *wo = BRWORLDOBJECTPOINTER( &args[0] );

	target->set( wo->getLightExposure() );

	return EC_OK;
}

/*!
	\brief Removes an object from the world and frees it.

	removeObject(slWorldObject pointer).
*/

int brIRemoveObject( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->removeObject( BRWORLDOBJECTPOINTER( &args[0] ) );

	delete BRWORLDOBJECTPOINTER( &args[0] );

	return EC_OK;
}

/*!
	\brief Enables or disables drawing of the bounding box for an object.

	setBoundingBox(slWorldObject pointer, int).
*/

int brISetBoundingBox( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int value = BRINT( &args[1] );

	if ( value ) o->addDrawMode( DM_BOUND );
	else o->removeDrawMode( DM_BOUND );

	return EC_OK;
}

/*!
	\brief Enables or disables drawing of the axis lines for an object.

	setBoundingBox(slWorldObject pointer, int).
*/

int brISetDrawAxis( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int value = BRINT( &args[1] );

	if ( !o ) {
		slMessage( DEBUG_ALL, "null pointer passed to setDrawAxis\n" );
		return EC_OK;
	}

	if ( value ) o->addDrawMode( DM_AXIS );
	else o->removeDrawMode( DM_AXIS );

	return EC_OK;
}

int brISetNeighborLines( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int value = BRINT( &args[1] );

	if ( value ) o->addDrawMode( DM_NEIGHBOR_LINES );
	else o->removeDrawMode( DM_NEIGHBOR_LINES );

	return EC_OK;
}

int brISetVisible( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int visible = BRINT( &args[1] );

	if ( !visible ) o->addDrawMode( DM_INVISIBLE );
	else o->removeDrawMode( DM_INVISIBLE );

	return EC_OK;
}

int brISetTexture( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int value = BRINT( &args[1] );

	o->setTexture( value );
	o->setTextureMode( BBT_NONE );

	return EC_OK;
}

int brISetTextureScale( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	float valueX = BRDOUBLE( &args[1] );
	float valueY = BRDOUBLE( &args[2] );

	if ( valueX < 0.0 ) {
		slMessage( DEBUG_ALL, "warning: texture scale must be positive (%f)\n", valueX );
		valueX = 0.001;
	}

	if ( valueY < 0.0 ) {
		slMessage( DEBUG_ALL, "warning: texture scale must be positive (%f)\n", valueY );
		valueY = 0.001;
	}

	o->setTextureScale( valueX, valueY );

	return EC_OK;
}

int brISetBitmap( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int texture = BRINT( &args[1] );

	o->setTexture( texture );
	o->setTextureMode( BBT_BITMAP );

	return EC_OK;
}

int brISetBitmapRotation( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );

	o->setBitmapRotation( RADTODEG( BRDOUBLE( &args[1] ) ) );

	return EC_OK;
}

/*
	step 1: compute the vector projection on the viewer's perspective plane.
			this is c . (nc + v) where c is the camera vector, v is the
			vector in question, and n is some constant.

			it becomes n * c.c + c.v = 0

			c.c = 1, so n = -c.v

*/

int brISetBitmapRotationTowardsVector( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	slVector *v = &BRVECTOR( &args[1] );
	slVector offset, vdiff, vproj, up, axis;
	double dot, rotation;

	slVectorSet( &up, 0, 1, 0 );

	slVectorCopy( &i->engine->camera->_location, &offset );
	slVectorNormalize( &offset );
	slVectorNormalize( v );

	slVectorCross( &up, &offset, &axis );
	slVectorCross( &offset, &axis, &up );

	/* c.v = -n */

	dot = -slVectorDot( &offset, v );

	slVectorMul( &offset, dot, &vdiff );
	slVectorAdd( &vdiff, v, &vproj );

	// we need to get the scalar rotation of v about offset vector

	rotation = acos( slVectorDot( &up, &vproj ) );

	if ( slVectorDot( &axis, &vproj ) > 0.0 ) rotation = 2 * M_PI - rotation;

	o->setBitmapRotation( RADTODEG( rotation ) );

	return EC_OK;
}

int brISetAlpha( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );

	o->setAlpha( BRDOUBLE( &args[1] ) );

	return EC_OK;
}

int brISetLightmap( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	int value = BRINT( &args[1] );

	o->setTexture( value );
	o->setTextureMode( BBT_LIGHTMAP );

	return EC_OK;
}

/*!
	\brief Sets the color of an object.
*/

int brISetColor( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o = BRWORLDOBJECTPOINTER( &args[0] );
	slVector *color = &BRVECTOR( &args[1] );

	o->setColor( color );

	return EC_OK;
}

int brIGetMainCameraPointer( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->camera );

	return EC_OK;
}

int brICameraSetTarget( brEval args[], brEval *target, brInstance *i ) {
	slVector *tar = &BRVECTOR( &args[0] );

	slVectorCopy( tar, &i->engine->camera->_target );

	i->engine->camera->update();

	return EC_OK;
}

int brICameraSetOffset( brEval args[], brEval *target, brInstance *i ) {
	slVector *loc = &BRVECTOR( &args[0] );
	double x;

	i->engine->camera->_zoom = slVectorLength( loc );

	slVectorNormalize( loc );

	// don't ask me to explain the following.  please.
	// it took me a while to work out on paper and i didn't
	// save my notes.  i never learn.  never never.

	// and now, a year later, there's a bug in this code!
	// looks like i'll have to figure it out.

	// if we have a unit vector (0, 0, 1), what are the X and Y
	// rotations to get to the target vector?

	// how much of a rotation about the x-axis would we make to
	// get a Y value of loc->y?

	i->engine->camera->_rx = -asin( loc->y );

	// now--how much would we rotate the resulting vector in order
	// to get the X value where we want it?

	// now consider that we're at (0, .707, 0) and that the target
	// is (.707, .707, 0).  That's 90 degree turn away, but if we
	// just took asin(.707) we'd get 45.  We have to 'normalize'
	// the X value by dividing it from our own distance from the
	// (0, 1, 0) vector.

	x = loc->x / cos( i->engine->camera->_rx );

	if ( x > 1.0 ) x = 1.0;

	if ( x < -1.0 ) x = -1.0;

	i->engine->camera->_ry = asin( x );

	// there are two x-rot values which fulfill this situation:
	// (x, y, z) and (x, y, -z)

	if ( loc->z < 0.0 ) i->engine->camera->_ry = M_PI - i->engine->camera->_ry;

	i->engine->camera->update();

	return EC_OK;
}

/*!
	\brief Set the size of the main camera's text.

	void cameraSetTextScale(double x, double y).
*/

int brICameraSetTextScale( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_textScale = BRDOUBLE( &args[0] );
	return EC_OK;
}

/*!
	\brief Set a string for display in the main camera.

	void cameraSetText(string message, int slotNumber, double x, double y, vector color).
*/

int brICameraSetText( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->setCameraText( BRINT( &args[1] ), BRSTRING( &args[0] ), BRDOUBLE( &args[2] ), BRDOUBLE( &args[3] ), &BRVECTOR( &args[4] ) );

	return EC_OK;
}

/*!
	\brief Sets the camera _zoom.

	void cameraSetZoom(double zoom).
*/

int brICameraSetZoom( brEval args[], brEval *target, brInstance *i ) {
	double z = BRDOUBLE( &args[0] );

	i->engine->camera->_zoom = z;

	i->engine->camera->update();

	return EC_OK;
}

/*!
	\brief Returns the zoom of the camera.
*/

int brICameraGetZoom( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->camera->_zoom );

	return EC_OK;
}

/*!
	\brief Returns the current offset of the camera relative to its target.

	vector cameraGetOffset().
*/

int brICameraGetOffset( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->camera->_location );

	return EC_OK;
}

/*!
	\brief Returns the current target of the camera.

	vector cameraGetTarget().
*/

int brICameraGetTarget( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->camera->_target );

	return EC_OK;
}

/*!
	\brief Sets the background color of the display.

	vector setBackgroundColor().
*/

int brISetBackgroundColor( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setBackgroundColor( &BRVECTOR( &args[0] ) );
	return EC_OK;
}

int brISetBackgroundTextureColor( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setBackgroundTextureColor( &BRVECTOR( &args[0] ) );
	return EC_OK;
}

/*!
	\brief Sets the background texture.

	void setBackgroundTexture(image number).
*/

int brISetBackgroundTexture( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setBackgroundTexture( BRINT( &args[0] ), 0 );
	return EC_OK;
}

/*!
	\brief Sets the background image.

	void setBackgroundImage(image number).
*/

int brISetBackgroundImage( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setBackgroundTexture( BRINT( &args[0] ), 1 );
	return EC_OK;
}

/*!
	\brief Gets the position of the main light.

	vector getLightPosition().
*/

int brIGetLightPosition( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->camera->_lights[0].location );

	return EC_OK;
}

/*!
	\brief Sets a the position of the main light.

	setLightPosition(vector position).
*/

int brISetLightPosition( brEval args[], brEval *target, brInstance *i ) {
	slVectorCopy( &BRVECTOR( &args[0] ), &i->engine->camera->_lights[0].location );
	i->engine->camera->_lights[0].changed = 1;
	return EC_OK;
}

int brISetDetectLightExposure( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setDetectLightExposure( BRINT( &args[0] ) );
	return EC_OK;
}

int brISetDrawLightExposure( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setDrawLightExposure( BRINT( &args[0] ) );
	return EC_OK;
}

int brIGetLightExposureCamera( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->world->getLightExposureCamera( ) );

	return EC_OK;
}

/*!
	\brief Sets a the light exposure detection source.

	setLightExposureSource(vector color).
*/

int brISetLightExposureSource( brEval args[], brEval *target, brInstance *i ) {
	i->engine->world->setLightExposureSource( &BRVECTOR( &args[0] ) );
	return EC_OK;
}

/*!
	\brief Sets a the ambient color for the light.

	setLightAmbientColor(vector color).
*/

int brISetLightAmbientColor( brEval args[], brEval *target, brInstance *i ) {
	slVectorCopy( &BRVECTOR( &args[0] ), &i->engine->camera->_lights[0].ambient );
	i->engine->camera->_lights[0].changed = 1;
	return EC_OK;
}

/**
 *	\brief Sets a the diffuse color for the light.
 *	setLightDiffuseColor(vector color).
 */

int brISetLightDiffuseColor( brEval args[], brEval *target, brInstance *i ) {
	slVectorCopy( &BRVECTOR( &args[0] ), &i->engine->camera->_lights[0].diffuse );
	i->engine->camera->_lights[0].changed = 1;
	return EC_OK;
}

/*!
	\brief Sets a stationary object to be a shadow-catcher.

	The plane normal specifies which plane of the object should catch the
	shadows.

	void setShadowCatcher(slWorldObject pointer object, vector planeNormal).
*/

int brISetShadowCatcher( brEval args[], brEval *target, brInstance *i ) {
	slStationary *st = ( slStationary* )BRPOINTER( &args[0] );
	slVector *norm = &BRVECTOR( &args[1] );

	st->addDrawMode( DM_STENCIL );

	i->engine->camera->setShadowCatcher( st, norm );

	return EC_OK;
}

/*!
	\brief Enables or disables point drawing for an object.
*/

int brISetDrawAsPoint( brEval args[], brEval *target, brInstance *i ) {
	BRWORLDOBJECTPOINTER( &args[0] )->setDrawAsPoint( BRINT( &args[1] ) );
	return EC_OK;
}

/*!
	\brief Adds a rendered line between two objects.

	void addObjectLine(slWorldObject pointer source, slWorldObject pointer dest, vector color, int style).
*/

int brIAddObjectLine( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *src = BRWORLDOBJECTPOINTER( &args[0] );
	slWorldObject *dst = BRWORLDOBJECTPOINTER( &args[1] );
	slVector *color = &BRVECTOR( &args[2] );
	char *patternString = BRSTRING( &args[3] );
	int pattern = 0, n = 0;

	n = strlen( patternString );

	if ( n > 16 ) n = 16;

	while ( n-- ) {
		pattern <<= 1;
		pattern |= ( *( patternString++ ) == '-' );
	}

	target->set( i->engine->world->addObjectLine( src, dst, pattern, color ) );

	return EC_OK;
}

/*!
	\brief Sets the stipple pattern of an object line.
*/

int brIObjectLineSetStipple( brEval args[], brEval *target, brInstance *i ) {
	slObjectLine *line = ( slObjectLine* )BRPOINTER( &args[0] );
	char *patternString = BRSTRING( &args[1] );

	int n, pattern = 0;

	n = strlen( patternString );

	if ( n > 16 ) n = 16;

	while ( n-- ) {
		pattern <<= 1;
		pattern |= ( *( patternString++ ) == '-' );
	}

	line->setStipple( pattern );

	return EC_OK;
}

int brIObjectLineSetColor( brEval args[], brEval *target, brInstance *i ) {
	slObjectLine *line = ( slObjectLine* )BRPOINTER( &args[0] );
	slVector *color = &BRVECTOR( &args[ 1 ] );

	line->setColor( *color );

	return EC_OK;
}

int brIObjectLineSetTransparency( brEval args[], brEval *target, brInstance *i ) {
	slObjectLine *line = ( slObjectLine* )BRPOINTER( &args[0] );
	float transparency = BRDOUBLE( &args[ 1 ] );

	line->setTransparency( transparency );

	return EC_OK;
}

/*!
	\brief Removes a specific object line between two objects.

	void removeObjectLine(slWorldObject pointer source, slWorldObject pointer dest).
*/

int brIRemoveObjectLine( brEval args[], brEval *target, brInstance *i ) {
	slObjectConnection *line = ( slObjectConnection* )BRPOINTER( &args[0] );

	i->engine->world->removeConnection( line );

	return EC_OK;
}

/*!
	\brief Sets whether bounds-only collision detection should be used.

	void setBoundsOnlyCollisionDetection(int state).
*/

int brISetBoundsOnlyCollisionDetection( brEval args[], brEval *target, brInstance *i ) {
	int value = BRINT( &args[0] );

	i->engine->world->setBoundsOnlyCollisionDetection( value );

	return EC_OK;
}

/*!
	\brief Raytraces from an object in a given direction -- implementation not complete [?]
*/

int brIRaytrace( brEval args[], brEval *target, brInstance *i ) {
	slWorldObject *o    = BRWORLDOBJECTPOINTER( &args[0] );
	slVector *location  = &BRVECTOR( &args[1] );
	slVector *direction = &BRVECTOR( &args[2] );

	slVector result;

	int error = o->raytrace( location, direction, &result );

	target->set( result );

	if ( error == -2 ) return EC_ERROR;

	return EC_OK;
}

/*!
	\brief returns the value a IRSensor would return at the position location and the direction only objects in the neighborhood of the worldobject are detected
	
	double slWorldObject::irSense(slVector* direction, slVector* up, std::string sensorType)

*/
int brIIRSense(brEval args[], brEval *target, brInstance *i) {
	slPosition sensorPos;
	slWorldObject *o    = BRWORLDOBJECTPOINTER(&args[0]);
	sensorPos.location = BRVECTOR(&args[1]); 
	slMatrixCopy(BRMATRIX(&args[2]), sensorPos.rotation); 

	double result = o->irSense(&sensorPos, BRSTRING(&args[3]));
	if (result == -1){
		return EC_ERROR;
	}
	target->set( result );

	return EC_OK;
}

//double slWorldObject::calculateQuality(slPosition* sensorPos, slVector* targetLoc, std::string sensorType){
int brIcalculateQualityToLoc(brEval args[], brEval *target, brInstance *i) {
	slVector targetLoc;
	slPosition sensorPos;
	slWorldObject *o    = BRWORLDOBJECTPOINTER(&args[0]);
	sensorPos.location = BRVECTOR(&args[1]); 
	slMatrixCopy(BRMATRIX(&args[2]), sensorPos.rotation); 
	targetLoc = BRVECTOR(&args[3]);
	double result = o->calculateQuality(&sensorPos, &targetLoc, BRSTRING(&args[4]));
	if (result == -1){
		return EC_ERROR;
	}
	target->set( result );

	return EC_OK;
}

//double slWorldObject::calculateQualityToObj(slPosition* sensorPos, slVector* targetLoc, std::string sensorType){
int brIcalculateQualityToObj(brEval args[], brEval *target, brInstance *i) {
	slWorldObject *targetObj;
	slVector targetLoc;
	slPosition sensorPos;
	slWorldObject *o    = BRWORLDOBJECTPOINTER(&args[0]);
	sensorPos.location = BRVECTOR(&args[1]); 
	slMatrixCopy(BRMATRIX(&args[2]), sensorPos.rotation); 
	targetObj = BRWORLDOBJECTPOINTER(&args[3]);
	double result = o->calculateQuality(&sensorPos, &targetLoc,  BRSTRING(&args[4]), targetObj);
	if (result == -1){
		return EC_ERROR;
	}
	target->set( result );

	return EC_OK;
}
//double slWorldObject::calculateQualityToObj(slPosition* sensorPos, slVector* targetLoc, std::string sensorType){
int brIcalcQualNoRay(brEval args[], brEval *target, brInstance *i) {
	slVector targetLoc;
	slPosition sensorPos;
	slWorldObject *o    = BRWORLDOBJECTPOINTER(&args[0]);
	sensorPos.location = BRVECTOR(&args[1]); 
	slMatrixCopy(BRMATRIX(&args[2]), sensorPos.rotation); 
	targetLoc = BRVECTOR(&args[3]);
	double result = o->calcQualNoRay(&sensorPos, &targetLoc, BRSTRING(&args[4]));
	if (result == -1){
		return EC_ERROR;
	}
	target->set( result );

	return EC_OK;
}

	/*
   * This function convertes a list of double and/or ints
   * to a double array.
   */

void listToDoubleArray(const brEvalListHead* list, double* result){
	std::vector< brEval > v = list->getVector();

	for( unsigned int i=0; i< v.size(); i++ ){
		if( v[i].type() == AT_DOUBLE ){
			result[i] = v[i].getDouble();
		}
	else if( v[i].type() == AT_INT ){
			result[i] = v[i].getInt();
		}
	else 
		{
			slMessage(DEBUG_ALL, "List of Doubles and/or Ints needed! (function listToDoubleArray)\n");
			printf("eval.type()= %d \n",v[i].type());
		}
//			printf("result[%d]= %f\n", i, result[i]);

	}
}


	/*
    * SensorBuilder::createSensor(const char* name, const int rows, const int columns, const double max_range, const double max_angle, 
	*			const int distance_length, const double* distance, const double* distance_factor,
	*			const int azimut_length, const double* azimut, const double* azimut_factor,
	*			const int incidence_length, const double* incidence, const double* incidence_factor){
	*
	*/
int brICreateUserSensor(brEval args[], brEval *target, brInstance *i) {
	//*(brEvalListHead **)variable = BRLIST(e);
	brEvalListHead* list;
	double* distance;
	double* distance_factor;
	double* azimut;
	double* azimut_factor;
	double* incidence;
	double* incidence_factor;

	list = BRLIST(&args[6]);
	distance = new double [list->getVector().size()];
	listToDoubleArray(list, distance);

	list = BRLIST(&args[7]);
	distance_factor = new double [list->getVector().size()];
	listToDoubleArray(list, distance_factor);
	list = BRLIST(&args[9]);
	azimut = new double [list->getVector().size()];
	listToDoubleArray(list, azimut);
	list = BRLIST(&args[10]);
	azimut_factor = new double [list->getVector().size()];
	listToDoubleArray(list, azimut_factor);
	list = BRLIST(&args[12]);
	incidence = new double [list->getVector().size()];
	listToDoubleArray(list, incidence);
	list = BRLIST(&args[13]);
	incidence_factor = new double [list->getVector().size()];
	listToDoubleArray(list, incidence_factor);

	//rows = columns and odd
	if((BRINT(&args[1])!=BRINT(&args[2]))||(BRINT(&args[1])%2!=1)){
		 slMessage(0, "Error creating a UserSensor: rows!=columns or rows=even");
		 return EC_ERROR;
	}
	double maxrange = BRDOUBLE(&args[3]);
	double maxangle = BRDOUBLE(&args[4]);
	if(maxangle > 79/M_PI*180) {
		 slMessage(0, "Error creating a UserSensor: maxangle>79");
		return EC_ERROR;
	}
	SensorBuilder::createUserSensor(BRSTRING(&args[0]), BRINT(&args[1]), BRINT(&args[2]),maxrange, maxangle, 
				BRINT(&args[5]), distance, distance_factor,
				BRINT(&args[8]), azimut, azimut_factor,
				BRINT(&args[11]), incidence, incidence_factor);
	delete[] distance;
	delete[] distance_factor;
	delete[] azimut;
	delete[] azimut_factor;
	delete[] incidence;
	delete[] incidence_factor;

	return EC_OK;
}

 

/*@}*/

/*!
	\brief Initializes internal breve functions related to the simulated world.
*/

void breveInitWorldFunctions( brNamespace *n ) {
	brNewBreveCall( n, "worldLoadTigerFile", brIWorldLoadTigerFile, AT_POINTER, AT_STRING, AT_POINTER, 0 );

	brNewBreveCall( n, "worldObjectGetLightExposure", brIWorldObjectGetLightExposure, AT_INT, AT_POINTER, 0 );

	brNewBreveCall( n, "setStepFast", brISetStepFast, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setStepFastIterations", brISetStepFastIterations, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setAutoDisableFlag", brISetAutoDisableFlag, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawEveryFrame", brISetDrawEveryFrame, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "randomSeed", brIRandomSeed, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "randomSeedFromDevRandom", brIRandomSeedFromDevRandom, AT_INT, 0 );
	brNewBreveCall( n, "getTime", brIGetTime, AT_DOUBLE, 0 );
	brNewBreveCall( n, "worldStep", brIWorldStep, AT_DOUBLE, AT_DOUBLE, AT_DOUBLE, 0 );
	brNewBreveCall( n, "updateNeighbors", brIUpdateNeighbors, AT_NULL, 0 );
	brNewBreveCall( n, "getNeighbors", brIGetNeighbors, AT_LIST, AT_POINTER, 0 );
	brNewBreveCall( n, "setNeighborhoodSize", brISetNeighborhoodSize, AT_NULL, AT_POINTER, AT_DOUBLE, 0 );

	brNewBreveCall( n, "removeObject", brIRemoveObject, AT_NULL, AT_POINTER, 0 );

	brNewBreveCall( n, "setBoundingBox", brISetBoundingBox, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setDrawAxis", brISetDrawAxis, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setNeighborLines", brISetNeighborLines, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setVisible", brISetVisible, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setTexture", brISetTexture, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setTextureScale", brISetTextureScale, AT_NULL, AT_POINTER, AT_DOUBLE, AT_DOUBLE, 0 );
	brNewBreveCall( n, "setBitmap", brISetBitmap, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setBitmapRotation", brISetBitmapRotation, AT_NULL, AT_POINTER, AT_DOUBLE, 0 );
	brNewBreveCall( n, "setBitmapRotationTowardsVector", brISetBitmapRotationTowardsVector, AT_NULL, AT_POINTER, AT_VECTOR, 0 );
	brNewBreveCall( n, "setAlpha", brISetAlpha, AT_NULL, AT_POINTER, AT_DOUBLE, 0 );
	brNewBreveCall( n, "setLightmap", brISetLightmap, AT_NULL, AT_POINTER, AT_INT, 0 );
	brNewBreveCall( n, "setColor", brISetColor, AT_NULL, AT_POINTER, AT_VECTOR, 0 );

	brNewBreveCall( n, "loadTexture", brILoadTexture, AT_INT, AT_STRING, AT_INT, 0 );

	brNewBreveCall( n, "getMainCameraPointer", brIGetMainCameraPointer, AT_POINTER, 0 );
	brNewBreveCall( n, "cameraSetOffset", brICameraSetOffset, AT_NULL, AT_VECTOR, 0 );
	brNewBreveCall( n, "cameraSetTarget", brICameraSetTarget, AT_NULL, AT_VECTOR, 0 );

	brNewBreveCall( n, "cameraSetZoom", brICameraSetZoom, AT_NULL, AT_DOUBLE, 0 );
	brNewBreveCall( n, "cameraGetZoom", brICameraGetZoom, AT_DOUBLE, 0 );
	brNewBreveCall( n, "cameraGetTarget", brICameraGetTarget, AT_VECTOR, 0 );
	brNewBreveCall( n, "cameraGetOffset", brICameraGetOffset, AT_VECTOR, 0 );
	brNewBreveCall( n, "cameraSetText", brICameraSetText, AT_NULL, AT_STRING, AT_INT, AT_DOUBLE, AT_DOUBLE, AT_VECTOR, 0 );
	brNewBreveCall( n, "cameraSetTextScale", brICameraSetTextScale, AT_NULL, AT_DOUBLE, 0 );

	brNewBreveCall( n, "getLightPosition", brIGetLightPosition, AT_VECTOR, 0 );
	brNewBreveCall( n, "setLightPosition", brISetLightPosition, AT_NULL, AT_VECTOR, 0 );
	brNewBreveCall( n, "setDetectLightExposure", brISetDetectLightExposure, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawLightExposure", brISetDrawLightExposure, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setLightExposureSource", brISetLightExposureSource, AT_NULL, AT_VECTOR, 0 );
	brNewBreveCall( n, "getLightExposureCamera", brIGetLightExposureCamera, AT_POINTER, 0 );

	brNewBreveCall( n, "setBackgroundColor", brISetBackgroundColor, AT_NULL, AT_VECTOR, 0 );
	brNewBreveCall( n, "setBackgroundTexture", brISetBackgroundTexture, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setBackgroundImage", brISetBackgroundImage, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setBackgroundTextureColor", brISetBackgroundTextureColor, AT_NULL, AT_VECTOR, 0 );

	brNewBreveCall( n, "setShadowCatcher", brISetShadowCatcher, AT_NULL, AT_POINTER, AT_VECTOR, 0 );
	brNewBreveCall( n, "setDrawAsPoint", brISetDrawAsPoint, AT_NULL, AT_POINTER, AT_INT, 0 );

	brNewBreveCall( n, "setLightAmbientColor", brISetLightAmbientColor, AT_NULL, AT_VECTOR, 0 );
	brNewBreveCall( n, "setLightDiffuseColor", brISetLightDiffuseColor, AT_NULL, AT_VECTOR, 0 );

	brNewBreveCall( n, "addObjectLine", brIAddObjectLine, AT_POINTER, AT_POINTER, AT_POINTER, AT_VECTOR, AT_STRING, 0 );
	brNewBreveCall( n, "removeObjectLine", brIRemoveObjectLine, AT_NULL, AT_POINTER, 0 );

	brNewBreveCall( n, "objectLineSetStipple", brIObjectLineSetStipple, AT_NULL, AT_POINTER, AT_STRING, 0 );
	brNewBreveCall( n, "objectLineSetColor", brIObjectLineSetColor, AT_NULL, AT_POINTER, AT_VECTOR, 0 );
	brNewBreveCall( n, "objectLineSetTransparency", brIObjectLineSetTransparency, AT_NULL, AT_POINTER, AT_DOUBLE, 0 );


	brNewBreveCall( n, "setBoundsOnlyCollisionDetection", brISetBoundsOnlyCollisionDetection, AT_NULL, AT_INT, 0 );

	brNewBreveCall( n, "setCollisionProperties", brISetCollisionProperties, AT_NULL, AT_POINTER, AT_DOUBLE, AT_DOUBLE, AT_DOUBLE, 0 );

	brNewBreveCall( n, "raytrace", brIRaytrace, AT_VECTOR, AT_POINTER, AT_VECTOR, AT_VECTOR, 0 );

	brNewBreveCall(n, "irSense", brIIRSense, AT_DOUBLE, AT_POINTER, AT_VECTOR, AT_MATRIX, AT_STRING, 0);
	brNewBreveCall(n, "calculateQualityToLocation", brIcalculateQualityToLoc, AT_DOUBLE, AT_POINTER, AT_VECTOR, AT_MATRIX, AT_VECTOR, AT_STRING,0);
	brNewBreveCall(n, "calculateQualityToObject", brIcalculateQualityToObj, AT_DOUBLE, AT_POINTER, AT_VECTOR, AT_MATRIX, AT_POINTER, AT_STRING,0);
	brNewBreveCall(n, "calcQualNoRay", brIcalcQualNoRay, AT_DOUBLE, AT_POINTER, AT_VECTOR, AT_MATRIX, AT_VECTOR, AT_STRING,0);

	brNewBreveCall(n, "createUserSensor", brICreateUserSensor, AT_NULL, AT_STRING, AT_INT, AT_INT, AT_DOUBLE, AT_DOUBLE,
																 AT_INT, AT_LIST, AT_LIST,
																 AT_INT, AT_LIST, AT_LIST,
																 AT_INT, AT_LIST, AT_LIST, 0);

}
