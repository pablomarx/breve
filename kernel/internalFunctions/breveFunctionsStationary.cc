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

/*! \addtogroup InternalFunctions */
/*@{*/

int brIAddStationary( brEval args[], brEval *target, brInstance *i ) {
	slShape *sh = ( slShape* )BRPOINTER( &args[0] );

	target->set( i->engine->world->addObject(
	                 new slStationary( sh, &BRVECTOR( &args[1] ), BRMATRIX( &args[2] ), i ) ) );

	return EC_OK;
}

/*@}*/

void breveInitStationaryFunctions( brNamespace *n ) {
	brNewBreveCall( n, "addStationary", brIAddStationary, AT_POINTER, AT_POINTER, AT_VECTOR, AT_MATRIX, 0 );
}
