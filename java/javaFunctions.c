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

#include "java.h"

#if HAVE_LIBJAVA

int brJavaMethodCall(brJavaBridgeData *, brJavaInstance *, brJavaMethod *, jvalue *, brEval *);

brJavaBridgeData *brAttachJavaVM(brEngine *e) {
	JavaVMInitArgs vm_args;
	JavaVMOption options[2];
	brJavaObject *finderObject;
	jint res;
	jmethodID method;
	char args[JAVA_MAX_ARGS];
	char *optstr;
	char *finder;
	char *classPath;

	brJavaBridgeData *bridge;

	finder = brFindFile(e, "MethodFinder.jar", NULL);

	if (!finder) {
		slMessage(DEBUG_WARN, "Java initialization failed: cannot locate Java utility archives\n");
		return NULL;
	}

	bridge = new brJavaBridgeData;

	classPath = getenv("BREVE_CLASS_PATH");

	if (!classPath)
		classPath = "";

	optstr = new char[(strlen(finder) + strlen(classPath) + strlen("-Djava.object.path=") + 1024)];

	sprintf(optstr, "-Djava.class.path=%s:%s:.", finder, classPath);

	slMessage(DEBUG_GEN, "opts: %s\n", optstr);

	vm_args.version = JNI_VERSION_1_2;

	JNI_GetDefaultJavaVMInitArgs(&vm_args);

	options[0].optionString = optstr;
	// options[1].optionString = "-verbose:jni,class";
	vm_args.options = options;
	vm_args.nOptions = 1;
	vm_args.ignoreUnrecognized = JNI_FALSE;

	// Create the Java VM 
	res = JNI_CreateJavaVM(&bridge->jvm, (void**)&bridge->env, &vm_args);

	delete[] optstr;

	if (res < 0) {
		slMessage(DEBUG_ALL, "Can't create JavaVM\n");
		slFree(bridge);
		return NULL;
	}

	if (!(finderObject = brJavaObjectFind(bridge, "MethodFinder"))) {
		slMessage(DEBUG_ALL, "Cannot locate MethodFinder class\n");
		return NULL;
	}

	bridge->methodFinder = brJavaBootstrapMethodFinder(finderObject);

	if (!bridge->methodFinder) {
		slMessage(DEBUG_ALL, "Cannot instantiate MethodFinder class\n");
		slFree(bridge);
		return NULL;
	}

	// method finder

	args[0] = 'O'; args[1] = 'O'; args[2] = 'I'; args[3] = 'O';
	method = (*(bridge->env)).GetMethodID(bridge->methodFinder->object->object, METHFIND_NAME, METHFIND_SIGNATURE);
	if (!method) {
		slMessage(DEBUG_ALL, "Cannot locate Java method \"%s\" for Java bridge\n", METHFIND_NAME);
		return NULL;
	}
	bridge->methfindMethod = brJavaMakeMethodData(METHFIND_NAME, method, 'O', args, 4);

	// method signature finder

	args[0] = 'O';
	method = (*bridge->env).GetMethodID(bridge->methodFinder->object->object, METHSIG_NAME, METHSIG_SIGNATURE);
	if(!method) {
		slMessage(DEBUG_ALL, "Cannot locate Java method \"%s\" for Java bridge\n", METHSIG_NAME);
		return NULL;
	}
	bridge->methsigMethod = brJavaMakeMethodData(METHSIG_NAME, method, 'T', args, 1);

	args[0] = 'O';
	method = (*bridge->env).GetMethodID(bridge->methodFinder->object->object, RETTYPE_NAME, RETTYPE_SIGNATURE);
	if(!method) {
		slMessage(DEBUG_ALL, "Cannot locate Java method \"%s\" for Java bridge\n", RETTYPE_NAME);
		return NULL;
	}
	bridge->rettypeMethod = brJavaMakeMethodData(RETTYPE_NAME, method, 'C', args, 2);

	args[0] = 'O'; args[1] = 'I';
	method = (*bridge->env).GetMethodID(bridge->methodFinder->object->object, ARGTYPES_NAME, ARGTYPES_SIGNATURE);
	if(!method) {
		slMessage(DEBUG_ALL, "Cannot locate Java method \"%s\" for Java bridge\n", ARGTYPES_NAME);
		return NULL;
	}
	bridge->argtypesMethod = brJavaMakeMethodData(ARGTYPES_NAME, method, 'C', args, 2);

	args[0] = 'O';
	method = (*bridge->env).GetMethodID(bridge->methodFinder->object->object, ARGCOUNT_NAME, ARGCOUNT_SIGNATURE);
	if(!method) {
		slMessage(DEBUG_ALL, "Cannot locate Java method \"%s\" for Java bridge\n", ARGCOUNT_NAME);
		return NULL;
	}
	bridge->argcountMethod = brJavaMakeMethodData(ARGCOUNT_NAME, method, 'I', args, 1);

	args[0] = 'O';
	method = (*bridge->env).GetMethodID(bridge->methodFinder->object->object, CLASSNAME_NAME, CLASSNAME_SIGNATURE);
	if(!method) {
		slMessage(DEBUG_ALL, "Cannot locate Java method \"%s\" for Java bridge\n", CLASSNAME_NAME);
		return NULL;
	}
	bridge->objectnameMethod = brJavaMakeMethodData(CLASSNAME_NAME, method, 'T', args, 1);

	slFree(finder);

	return bridge;
}

void brDetachJavaVM(brJavaBridgeData *bridge) {
	brFreeJavaBridgeData(bridge);
}

void brFreeJavaClassData(brJavaObject *data) {
	delete data;
}

brJavaMethod *brJavaMethodFind(brJavaBridgeData *bridge, brJavaObject *object, char *name, unsigned char *types, unsigned int nargs) {
	char returnType, argumentTypes[JAVA_MAX_ARGS];
	jmethodID methodID;
	unsigned int count;
	int error;
	brEval result;
	jvalue args[JAVA_MAX_ARGS];
	char *signature;
	jobject methodPtr;
	static jcharArray array = NULL;
	jchar cargTypes[JAVA_MAX_ARGS];

	// translate to a valid java name
	name = slStrdup(name);
	for(count=0;count<strlen(name);count++) 
		if(name[count] == '-') name[count] = '_';

	if(!array) array = (*bridge->env).NewCharArray(JAVA_MAX_ARGS);

	for(count=0;count<nargs;count++) cargTypes[count] = brJTypeForType(types[count]);

	(*bridge->env).SetCharArrayRegion(array, 0, nargs, cargTypes);

	// find the method with this name and arg counts for this object
	args[0].l = object->object;
	args[1].l = brMakeJavaString(bridge, name);
	args[2].i = nargs;
	args[3].l = array;
	error = brJavaMethodCall(bridge, bridge->methodFinder, bridge->methfindMethod, args, &result);
	methodPtr = (jobject)BRPOINTER(&result);

	if(error != EC_OK) return NULL;

	// find the proper signature for the method
	args[0].l = methodPtr;
	error = brJavaMethodCall(bridge, bridge->methodFinder, bridge->methsigMethod, args, &result);
	signature = BRSTRING(&result);

	if(error != EC_OK) return NULL;

	methodID = (*bridge->env).GetMethodID(object->object, name, signature);
	slFree(signature);

	// find the return type for the method
	args[0].l = methodPtr;
	error = brJavaMethodCall(bridge, bridge->methodFinder, bridge->rettypeMethod, args, &result);
	returnType = BRINT(&result);

	if(error != EC_OK) return NULL;

	// fill in the argument types
	for(count=0;count<nargs;count++) {
		args[0].l = methodPtr;
		args[1].i = count;

		error = brJavaMethodCall(bridge, bridge->methodFinder, bridge->argtypesMethod, args, &result);
		argumentTypes[count] = BRINT(&result);

		if(error != EC_OK) return NULL;
	}

	slFree(name);

	return brJavaMakeMethodData(name, methodID, returnType, argumentTypes, nargs);
}

brJavaMethod *brJavaMakeMethodData(char *name, jmethodID method, char returnType, char *argumentTypes, int nargs) {
	brJavaMethod *data;
	int n;

	data = new brJavaMethod;

	data->method = method;
	data->argumentCount = nargs;
	data->returnType = returnType;
	data->name = slStrdup(name);

	for(n=0;n<nargs;n++) data->argumentTypes[n] = argumentTypes[n];

	return data;
}

int brJavaMethodCall(brJavaBridgeData *bridge, brJavaInstance *instance, brJavaMethod *method, jvalue *jargs, brEval *result) {
	jvalue returnValue;

	switch( method->returnType ) {
		case 'V':
			(*bridge->env).CallVoidMethodA(instance->instance, method->method, jargs);
			break;
		case 'I':
			returnValue.i = (*bridge->env).CallIntMethodA(instance->instance, method->method, jargs);
			result->set( (int)returnValue.i );
			break;
		case 'J':
			returnValue.j = (*bridge->env).CallLongMethodA(instance->instance, method->method, jargs);
			result->set( (int)returnValue.j );
			break;
		case 'C':
			returnValue.c = (*bridge->env).CallCharMethodA(instance->instance, method->method, jargs);
			result->set( returnValue.c );
			break;
		case 'B':
			returnValue.b = (*bridge->env).CallByteMethodA(instance->instance, method->method, jargs);
			result->set( returnValue.b );
			break;
		case 'Z':
			returnValue.z = (*bridge->env).CallBooleanMethodA(instance->instance, method->method, jargs);
			result->set( returnValue.z );
			break;
		case 'S':
			returnValue.s = (*bridge->env).CallShortMethodA(instance->instance, method->method, jargs);
			result->set( returnValue.s );
			break;
		case 'F':
			returnValue.f = (*bridge->env).CallFloatMethodA(instance->instance, method->method, jargs);
			result->set( returnValue.f );
			break;
		case 'D':
			returnValue.d = (*bridge->env).CallDoubleMethodA(instance->instance, method->method, jargs);
			result->set( returnValue.d );
			break;
		case 'T':
			// 'T' is breve's code for a string object
			returnValue.l = (*bridge->env).CallObjectMethodA(instance->instance, method->method, jargs);
			result->set( brReadJavaString(bridge, (jstring)returnValue.l) );
			break;
		case 'O':
			// 'O' is breve's code for an object
			returnValue.l = (*bridge->env).CallObjectMethodA(instance->instance, method->method, jargs);
			result->set( (void*)returnValue.l );
			break;
		default:
			slMessage(DEBUG_ALL, "error: undefined Java type (%c) returned from method \"\"\n", method->returnType);
			break;
	}

	// check for exception here
	
	if((*bridge->env).ExceptionOccurred()) {
		(*bridge->env).ExceptionDescribe();
		(*bridge->env).ExceptionClear();
		slMessage(DEBUG_ALL, "Exception occured in Java execution of method \"%s\"\n", method->name);
		return EC_ERROR;			
	}

	return EC_OK;
}

brJavaObject *brJavaObjectFind(brJavaBridgeData *bridge, char *name) {
	brJavaObject *object;
	unsigned int n;

	name = slStrdup(name);

	// translate to a valid java object name.

	for(n=0;n<strlen(name);n++) if(name[n] == '.') name[n] = '/';

	if(!(object = bridge->objects[ name])) {
		object = new brJavaObject;

		object->object = (*bridge->env).FindClass(name);

		if(!object->object) {
			if ((*bridge->env).ExceptionOccurred()) {
				(*bridge->env).ExceptionClear();
			}

			return NULL;
		}

		object->bridge = bridge;
	}

	slFree(name);

	return object;
}

int brJavaInstanceFree(brJavaInstance *i) {
	delete i;

	return EC_OK;
}

brJavaInstance *brJavaBootstrapMethodFinder(brJavaObject *object) {
	brJavaInstance *instance = new brJavaInstance;

	jmethodID method = (*object->bridge->env).GetMethodID(object->object, "<init>", "()V"); 

	instance->object = object;
	instance->instance = (*object->bridge->env).NewObjectA(object->object, method, NULL);

	return instance;
}

brJavaInstance *brJavaInstanceNew(brJavaObject *object, brEval **args, int argCount) {
	brJavaInstance *instance;
	jvalue jargs[JAVA_MAX_ARGS];
	brJavaMethod *method;
	unsigned char types[JAVA_MAX_ARGS];
	int n;

	instance = new brJavaInstance;

	instance->object = object;

	for(n=0;n<argCount;n++) types[n] = args[n]->type();

	method = brJavaMethodFind(object->bridge, instance->object, "<init>", types, argCount);

	for(n=0;n<argCount;n++) {
		if(brEvalToJValue(object->bridge, args[n], &jargs[n], method->argumentTypes[n]) != EC_OK) {
			slMessage(DEBUG_ALL, "Error converting breve type to Java native type\n");
			return NULL;
		}
	}

	instance->instance = (*object->bridge->env).NewObjectA(object->object, method->method, jargs);

	if(!instance->instance) {
		slFree(instance);
		return NULL;
	}

	return instance;
}

void brFreeJavaMethodData(brJavaMethod *method) {
	slFree(method->name);
	delete method;
}

void brFreeJavaBridgeData(brJavaBridgeData *bridge) {
	brFreeJavaMethodData(bridge->methfindMethod);
	brFreeJavaMethodData(bridge->methsigMethod);
	brFreeJavaMethodData(bridge->argtypesMethod);
	brFreeJavaMethodData(bridge->argcountMethod);
	brFreeJavaMethodData(bridge->rettypeMethod);
	brFreeJavaMethodData(bridge->objectnameMethod);
	delete bridge->methodFinder;
	delete bridge;
}
#endif
