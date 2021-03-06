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

#define PIPE_READ_SIZE	1024

/*! \addtogroup InternalFunctions */
/*@{*/

#include <unistd.h>
#include "kernel.h"
#include "world.h"
#include "camera.h"

/*!
	\brief Gets all the keys pressed down.
*/

int brIGetAllPressedKeys( brEval args[], brEval *target, brInstance *i ) {
	char down[257];
	int index = 0;
	int n;

	for ( n = 0;n < 256;n++ ) {
		if ( i->engine->keys[n] > 0 ) {
			i->engine->keys[n]--;
			down[index++] = n;
		}
	}

	down[index] = 0;

	target->set( down );

	return EC_OK;
}

/*!
	\brief Gets the mouse x-coordinate.
*/

int brIGetMouseX( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->mouseX );

	return EC_OK;
}

/*!
	\brief Gets the mouse y-coordinate.
*/

int brIGetMouseY( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->mouseY );

	return EC_OK;
}

/*!
	\brief Causes the simulation to stop gracefully at the end of the iteration
	step.

	void endSimulation()
*/

int brIEndSimulation( brEval args[], brEval *target, brInstance *i ) {
	i->engine->simulationWillStop = 1;
	return EC_OK;
}

/*!
	\brief Executes a system() call and returns the output of the command.

	string system(string command).
*/

int brISystem( brEval args[], brEval *target, brInstance *i ) {
	FILE *stream;
	char *result;
	int totalSize, position, totalRead;

	char *command = BRSTRING( &args[0] );

	stream = popen( command, "r" );

	if ( !stream ) {
		slMessage( DEBUG_ALL, "system call \"%s\" failed\n", command );
		return EC_ERROR;
	}

	result = ( char* )slMalloc( PIPE_READ_SIZE );

	position = 0;
	totalSize = PIPE_READ_SIZE;

	do {
		totalRead = slUtilFread( &result[position], totalSize - position, 1, stream );

		if ( totalRead == PIPE_READ_SIZE ) {
			position = totalSize;
			totalSize += PIPE_READ_SIZE;
			command = ( char* )slRealloc( result, totalSize );
		}
	} while ( totalRead == PIPE_READ_SIZE );

	target->set( result );

	pclose( stream );

	return EC_OK;
}

/*!
	\brief Causes the simulation to sleep for a specified time.

	The sleep time is specified in seconds, but is a double, so it
	does not have to be a whole number of seconds.

	void sleep(double seconds).
*/

int brISleep( brEval args[], brEval *target, brInstance *i ) {
	usleep(( int )( 1000000 * BRDOUBLE( &args[0] ) ) );

	return EC_OK;
}

/*!
	\brief Causes the simulation to pause, if supported by the interface.

	Pauses the simulation as though the user had requested that the simulation
	pause.  This function has no effect if the interface does not support
	pausing.  In particular, the breve_cli program does not support pausing.

	void pause().
*/

int brIPause( brEval args[], brEval *target, brInstance *i ) {
	if ( i->engine->pauseCallback ) i->engine->pauseCallback();

	return EC_OK;
}

/*!
	\brief Causes the simulation to pause, if supported by the interface.

	Pauses the simulation as though the user had requested that the simulation
	pause.  This function has no effect if the interface does not support
	pausing.  In particular, the breve_cli program does not support pausing.

	void pause().
*/

int brIUnpause( brEval args[], brEval *target, brInstance *i ) {
	if ( i->engine->unpauseCallback ) i->engine->unpauseCallback();

	return EC_OK;
}

/*!
	\brief Add an event to the breve engine to be executed at a certain time.

	The method will be executed for the calling object.

	void addEvent(double time, string methodName).
*/

int brIAddEvent( brEval args[], brEval *target, brInstance *i ) {
	if ( BRDOUBLE( &args[1] ) <= i->engine->world->getAge( ) ) {
		slMessage( DEBUG_ALL, "warning: attempt to add event to the past\n" );
		return EC_OK;
	}

	if ( !brEngineAddEvent( i->engine, i, BRSTRING( &args[0] ), BRDOUBLE( &args[1] ), BRDOUBLE( &args[2] ) ) ) {
		slMessage( DEBUG_ALL, "unable to add new event to engine: brEngineAddEvent() failed\n" );
		return EC_ERROR;
	}

	return EC_OK;
}

/*!
	\brief Set the starting and ending distances for fog.

	void setFogDistances(double start, double end).
*/

int brISetFogDistances( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_fogStart = BRDOUBLE( &args[0] );
	i->engine->camera->_fogEnd = BRDOUBLE( &args[1] );
	return EC_OK;
}

/*!
    \brief Sets OpenGL lighting for the main camera.

	void setDrawLights(int lights).
*/

int brISetDrawLights( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_drawLights = BRINT( &args[0] );
	i->engine->camera->setRecompile();
	return EC_OK;
}

/*!
    \brief Sets OpenGL shadows for the main camera.

	void drawShadow(int shadow).
*/

int brISetDrawShadow( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_drawShadow = BRINT( &args[0] );
	i->engine->camera->setRecompile();
	return EC_OK;
}

/*!
    \brief Sets OpenGL shadow-volumes (improved shadows) for the main camera.

	void drawShadowVolumes(int shadow).
*/

int brISetDrawShadowVolumes( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_drawShadowVolumes = BRINT( &args[0] );
	i->engine->camera->setRecompile();
	return EC_OK;
}

/*!
    \brief Set OpenGL "outline" drawing for the main camera.

	Outline drawing is basically a black & white wireframe style.
	Useful for creating black & white print quality images.

	void setDrawOutline(int outline).
*/

int brISetDrawOutline( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_drawOutline = BRINT( &args[0] );
	i->engine->camera->setRecompile();
	return EC_OK;
}

/*!
    \brief Sets OpenGL reflection for the main camera.

	void setDrawReflection(int reflection).
*/

int brISetDrawReflection( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_drawReflection = BRINT( &args[0] );
	i->engine->camera->setRecompile();
	return EC_OK;
}

/*!
    \brief Sets the background display rate for the main camera.

	void setBackgroundScroll(double scrollx, double scrolly).
*/

int brISetBackgroundScroll( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_backgroundScrollX = BRDOUBLE( &args[0] );
	i->engine->camera->_backgroundScrollY = BRDOUBLE( &args[1] );

	return EC_OK;
}

/*!
	\brief Turns text drawing on/off for the main camera.

	void setDrawText(int text).
*/

int brISetDrawText( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_drawText = BRINT( &args[0] );

	return EC_OK;
}

/*!
	\brief Sets the z-clipping plane distance for the main camera.

	void setZClip(int distance).
*/

int brISetZClip( brEval args[], brEval *target, brInstance *i ) {
	i->engine->camera->_zClip = abs( BRINT( &args[0] ) );

	if ( i->engine->camera->_zClip == 0 ) i->engine->camera->_zClip = 1;

	return EC_OK;
}

/*!
	\brief Gets the current interface version string.

	string getInterfaceVersion().
*/

int brIGetInterfaceVersion( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->interfaceTypeCallback() );

	return EC_OK;
}

/*!
	\brief Presents a dialog box to the user, if supported by the interface.

	Gives the user two button options and returns the number corresponding
	to the button pressed.  The text and title of the dialog are passed in
	as strings, along with the text corresponding to the yes and no buttons.

	int dialogBox(string title, string message, string yesText, string noText).
*/

int brIDialogBox( brEval args[], brEval *target, brInstance *i ) {
	if ( !i->engine->dialogCallback ) {

		target->set( 0 );

		return EC_OK;
	}

	target->set( i->engine->dialogCallback( BRSTRING( &args[0] ), BRSTRING( &args[1] ), BRSTRING( &args[2] ), BRSTRING( &args[3] ) ) );

	return EC_OK;
}

/*!
	\brief Plays a system beep sound.

	void playSound().
*/

int brIPlaySound( brEval args[], brEval *target, brInstance *i ) {
	if ( !i->engine->soundCallback ) return EC_OK;

	i->engine->soundCallback();

	return EC_OK;
}

/*!
	\brief Gets the number of command-line arguments.

	int getArgc().
*/

int brIGetArgc( brEval args[], brEval *target, brInstance *i ) {

	target->set( i->engine->argc );

	return EC_OK;
}

/*!
	\brief Gets the command-line argument at the specified index.

	string getArgv(int index).
*/

int brIGetArgv( brEval args[], brEval *target, brInstance *i ) {
	if ( BRINT( &args[0] ) < 0 || BRINT( &args[0] ) >= i->engine->argc ) {
		slMessage( DEBUG_ALL, "Request for command-line input argument %d is out of range\n", i->engine->argc );
		return EC_ERROR;
	}

	target->set( i->engine->argv[BRINT( &args[0] )] );

	return EC_OK;
}

/*!
	\brief Sets the interface string value for the specified id value.

	int setInterfaceString(string value, int id).
*/

int brISetInterfaceString( brEval args[], brEval *target, brInstance *i ) {
	if ( i->engine->interfaceSetStringCallback ) {

		target->set( i->engine->interfaceSetStringCallback( BRSTRING( &args[0] ), BRINT( &args[1] ) ) );
	} else {

		target->set( 0 );
	}

	return EC_OK;
}

/*!
	\brief Returns a unique color vector for a number.

	vector uniqueColor(int number).
*/

int brIUniqueColor( brEval args[], brEval *target, brInstance *i ) {
	slVector v;
	brUniqueColor( &v, BRINT( &args[0] ) );

	target->set( v );

	return EC_OK;
}

/*!
	\brief Convert an HSV color vector to RGB.

	vector HSVtoRGB(vector color).
*/

int brIHSVtoRGB( brEval args[], brEval *target, brInstance *i ) {
	slVector v;
	brHSVtoRGB( &BRVECTOR( &args[0] ), &v );

	target->set( v );

	return EC_OK;
}

/*!
	\brief Convert an RGB color vector to HSV.

	vector RGBtoHSV(vector color).
*/

int brIRGBtoHSV( brEval args[], brEval *target, brInstance *i ) {
	slVector v;
	brRGBtoHSV( &BRVECTOR( &args[0] ), &v );

	target->set( v );

	return EC_OK;
}

/*!
	\brief Finds a file and retruns the entire file path using the engine's
	file path.

	string findFile(string file).
*/

int brIFindFile( brEval args[], brEval *target, brInstance *i ) {
	char *file = brFindFile( i->engine, BRSTRING( &args[0] ), NULL );

	if( file ) {
		target->set( file );
		slFree( file );
	} else {
		target->set( "" );
	}

	return EC_OK;
}

/*!
	\brief Returns the current real-time time with micro-second precision.
*/

int brIGetRealTime( brEval args[], brEval *target, brInstance *i ) {

	struct timeval now;
	double seconds;

	gettimeofday( &now, NULL );

	seconds = now.tv_sec + now.tv_usec / 1000000.0;

	target->set( seconds );

	return EC_OK;
}

/**
 * Sets the output filter level, one of \ref slDebugLevels.
 */

int brISetOutputFilter( brEval args[], brEval *target, brInstance *i ) {
	slSetDebugLevel( BRINT( &args[0] ) );

	return EC_OK;
}

/**
 * Why would I even let such a function exist?  It's downright
 * irresponsible.
 */

int brICrash( brEval args[], brEval *target, brInstance *i ) {
	char *uhoh = NULL;

	*uhoh = 1;

	return EC_OK;
}

/*@}*/

// initialize the control related functions

void breveInitControlFunctions( brNamespace *n ) {
	brNewBreveCall( n, "endSimulation", brIEndSimulation, AT_NULL, 0 );

	brNewBreveCall( n, "system", brISystem, AT_STRING, AT_STRING, 0 );
	brNewBreveCall( n, "sleep", brISleep, AT_NULL, AT_DOUBLE, 0 );
	brNewBreveCall( n, "pauseSimulation", brIPause, AT_NULL, 0 );
	brNewBreveCall( n, "unpauseSimulation", brIUnpause, AT_NULL, 0 );

	brNewBreveCall( n, "addEvent", brIAddEvent, AT_NULL, AT_STRING, AT_DOUBLE, AT_DOUBLE, 0 );
	brNewBreveCall( n, "setFogDistances", brISetFogDistances, AT_NULL, AT_DOUBLE, AT_DOUBLE, 0 );
	brNewBreveCall( n, "setDrawLights", brISetDrawLights, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawShadow", brISetDrawShadow, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawShadowVolumes", brISetDrawShadowVolumes, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawOutline", brISetDrawOutline, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawText", brISetDrawText, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setDrawReflection", brISetDrawReflection, AT_NULL, AT_INT, 0 );
	brNewBreveCall( n, "setBackgroundScroll", brISetBackgroundScroll, AT_NULL, AT_DOUBLE, AT_DOUBLE, 0 );
	brNewBreveCall( n, "setZClip", brISetZClip, AT_NULL, AT_INT, 0 );

	brNewBreveCall( n, "getInterfaceVersion", brIGetInterfaceVersion, AT_STRING, 0 );
	brNewBreveCall( n, "dialogBox", brIDialogBox, AT_INT, AT_STRING, AT_STRING, AT_STRING, AT_STRING, 0 );
	brNewBreveCall( n, "playSound", brIPlaySound, AT_NULL, 0 );
	brNewBreveCall( n, "getArgv", brIGetArgv, AT_STRING, AT_INT, 0 );
	brNewBreveCall( n, "getArgc", brIGetArgc, AT_INT, 0 );

	brNewBreveCall( n, "setInterfaceString", brISetInterfaceString, AT_INT, AT_STRING, AT_INT, 0 );

	brNewBreveCall( n, "uniqueColor", brIUniqueColor, AT_VECTOR, AT_INT, 0 );
	brNewBreveCall( n, "RGBtoHSV", brIRGBtoHSV, AT_VECTOR, AT_VECTOR, 0 );
	brNewBreveCall( n, "HSVtoRGB", brIHSVtoRGB, AT_VECTOR, AT_VECTOR, 0 );

	brNewBreveCall( n, "getMouseX", brIGetMouseX, AT_INT, 0 );
	brNewBreveCall( n, "getMouseY", brIGetMouseY, AT_INT, 0 );

	brNewBreveCall( n, "getAllPressedKeys", brIGetAllPressedKeys, AT_STRING, 0 );

	brNewBreveCall( n, "findFile", brIFindFile, AT_STRING, AT_STRING, 0 );
	brNewBreveCall( n, "getRealTime", brIGetRealTime, AT_DOUBLE, 0 );

	brNewBreveCall( n, "setOutputFilter", brISetOutputFilter, AT_NULL, AT_INT, 0 );

	brNewBreveCall( n, "crash", brICrash, AT_NULL, 0 );
}
