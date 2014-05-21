/**
 *
 */
#include <stdbool.h>

#include <jni.h>
#include <jvmti.h>

#define TYPE_CLASS	0B0
#define STAMP_TYPE_OBJECT	0B1

#define TYPE_BITS	 1
#define THREAD_BITS	 8
#define CLASS_BITS	19
#define OBJECT_BITS	36

#if TYPE_BITS+THREAD_BITS+CLASS_BITS+OBJECT_BITS!=64
#error "TYPE_BITS+THREAD_BITS+CLASS_BITS+OBJECT_BITS must be 64!"
#endif

#define TYPE_MASK	( ( 1L << TYPE_BITS ) - 1 )
#define THREAD_MASK	( ( 1L << THREAD_BITS ) - 1 )
#define CLASS_MASK	( ( 1L << CLASS_BITS ) - 1 )
#define OBJECT_MASK	( ( 1L << OBJECT_BITS ) - 1 )

#define OBJECT_POS	( 0 )
#define CLASS_POS	( OBJECT_POS + OBJECT_BITS )
#define THREAD_POS	( CLASS_POS + CLASS_BITS )
#define TYPE_POS	( THREAD_POS + THREAD_BITS )

static inline jlong GetStampType(jlong stamp) {
	return (stamp >> TYPE_POS) & TYPE_MASK;
}

static inline jlong GetClassIdFromStamp(jlong classstamp) {
	return (classstamp >> CLASS_POS) & CLASS_MASK;
}

static inline bool StampIsClass(jlong stamp) {
	return stamp != 0L && GetStampType(stamp) == TYPE_CLASS && GetClassIdFromStamp(stamp) >= 1;
}

void StampThread(jvmtiEnv* jvmti, jthread thread);
jlong StampClass(jvmtiEnv* jvmti, JNIEnv* jni, jclass klass);
jlong StampObject(jvmtiEnv* jvmti, JNIEnv* jni, jobject object);
