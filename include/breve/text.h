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

#include <stdio.h>
#include <zlib.h>

char *slFileExtension( const char *inName );

char *slSplit(char *, char *, int);

int slUtilGzread(char *, int, gzFile);
int slUtilGzwrite(void *, size_t, size_t, gzFile);

int slUtilFread(void *, size_t, size_t, FILE *);
int slUtilFwrite(const void *, size_t, size_t, FILE *);

int slUtilRead(int, void *, size_t);
int slUtilRecv(int, void *, size_t);
int slUtilWrite(int, const void *, size_t);

char *slUtilReadFile( const char * );
int slUtilWriteFile( const char *, const char * );

char *slUtilReadStream(FILE *);

char *slDequote(char *);

char *slStrcatM(char *, char *);

char *slFgets(char *, int, FILE *);

char *slUtilReadStdin(void);
