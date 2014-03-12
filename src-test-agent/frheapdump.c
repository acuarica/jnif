/**
 *
 */
#include <stdbool.h>
#include <string.h>
#include <jvmti.h>

#include "frthread.h"
#include "frlog.h"
#include "frexception.h"
#include "frjvmti.h"
#include "frtlog.h"
#include "frstamp.h"
#include "frheapdump.h"

/**
 *
 */
typedef struct __FollowReferencesArgsTag {
	FILE* out;
	void* frbuf;
	//bool init;
	jlong javaLangClassTag;
	int sample;
} FollowReferencesArgs;

/**
 *
 */
static jint _nextsampleid = 1;

/**
 *
 */
static const char* get_heap_reference_kind_desc(jvmtiHeapReferenceKind kind) {
	switch (kind) {
		case JVMTI_HEAP_REFERENCE_CLASS:
			return "CLASS";
		case JVMTI_HEAP_REFERENCE_FIELD:
			return "FIELD";
		case JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT:
			return "ARRAY_ELEMENT";
		case JVMTI_HEAP_REFERENCE_CLASS_LOADER:
			return "CLASS_LOADER";
		case JVMTI_HEAP_REFERENCE_SIGNERS:
			return "SIGNERS";
		case JVMTI_HEAP_REFERENCE_PROTECTION_DOMAIN:
			return "PROTECTION_DOMAIN";
		case JVMTI_HEAP_REFERENCE_INTERFACE:
			return "INTERFACE";
		case JVMTI_HEAP_REFERENCE_STATIC_FIELD:
			return "STATIC_FIELD";
		case JVMTI_HEAP_REFERENCE_CONSTANT_POOL:
			return "CONSTANT_POOL";
		case JVMTI_HEAP_REFERENCE_SUPERCLASS:
			return "SUPERCLASS";
		case JVMTI_HEAP_REFERENCE_JNI_GLOBAL:
			return "JNI_GLOBAL";
		case JVMTI_HEAP_REFERENCE_SYSTEM_CLASS:
			return "SYSTEM_CLASS";
		case JVMTI_HEAP_REFERENCE_MONITOR:
			return "MONITOR";
		case JVMTI_HEAP_REFERENCE_STACK_LOCAL:
			return "STACK_LOCAL";
		case JVMTI_HEAP_REFERENCE_JNI_LOCAL:
			return "JNI_LOCAL";
		case JVMTI_HEAP_REFERENCE_THREAD:
			return "THREAD";
		case JVMTI_HEAP_REFERENCE_OTHER:
			return "OTHER";
		default:
			EXCEPTION("Invalid kind desc found: %d", kind);
			// TODO: Make eclipse doesn't bother for a nonreturn function.
			return NULL ;
	}
}

static bool ReferenceKindIsHeapRoot(jvmtiHeapReferenceKind reference_kind) {
	switch (reference_kind) {
		case JVMTI_HEAP_REFERENCE_JNI_GLOBAL:
		case JVMTI_HEAP_REFERENCE_SYSTEM_CLASS:
		case JVMTI_HEAP_REFERENCE_MONITOR:
		case JVMTI_HEAP_REFERENCE_STACK_LOCAL:
		case JVMTI_HEAP_REFERENCE_JNI_LOCAL:
		case JVMTI_HEAP_REFERENCE_THREAD:
		case JVMTI_HEAP_REFERENCE_OTHER:
			return true;
		default:
			return false;
	}
}

void PrintStamp(FILE* out, jlong stamp) {
	fprintf(out, ":%ld", stamp);
}

static void CheckJavaLangClassStamp(FollowReferencesArgs* args, jlong classTag,
		jlong javaLangClassTag) {
	ASSERT(classTag == 0L || StampIsClass(classTag),
			"classTag: %ld, javaLangClassTag: %ld", classTag, javaLangClassTag);

	if (args->javaLangClassTag == 0) {
		args->javaLangClassTag = javaLangClassTag;
	} else {
		ASSERT(javaLangClassTag == args->javaLangClassTag, "%ld != %ld",
				javaLangClassTag, args->javaLangClassTag);
	}
}

static jint heap_iteration_callback(jlong class_tag, jlong size, jlong* tag_ptr,
		jint length, void* user_data) {

	FollowReferencesArgs* args = (FollowReferencesArgs*) user_data;

	jlong tag = tag_ptr == NULL ? 0 : *tag_ptr;
	fprintf(args->out, "%ld:%ld:%ld:%i", class_tag, size, tag, length);

//fprintnetref(args->out, class_tag);
//fprintnetref(args->out, tag);

	fprintf(args->out, "\n");

	return JVMTI_VISIT_OBJECTS;
}

static jint JNICALL HeapReferenceCallback(jvmtiHeapReferenceKind refkind,
		const jvmtiHeapReferenceInfo* refinfo, jlong class_tag,
		jlong referrer_class_tag, jlong size, jlong* tag_ptr,
		jlong* referrer_tag_ptr, jint length, FollowReferencesArgs* args) {

	if (args->out == NULL ) {
		// TODO: Careful with race here. There should be no race inside FollowReferences.
		int sample = _nextsampleid;
		_nextsampleid++;

		char filename[512];
		sprintf(filename, "fr.%08d.log", sample);
		args->out = fopen(filename, "w");
		args->javaLangClassTag = 0;
		args->sample = sample;

		_TLOG("SAMPLE:%d:%s", sample, filename);
	}

	ASSERT(args->out != NULL, "Invalid init");

	ASSERT(tag_ptr != NULL, "");
	ASSERT(class_tag == 0L || StampIsClass(class_tag), "%ld", class_tag);
	ASSERT(tag_ptr != NULL, "");

	jlong tag = *tag_ptr;

	const char* kinddesc = get_heap_reference_kind_desc(refkind);

	fprintf(args->out, "%s", kinddesc);

	bool isroot = ReferenceKindIsHeapRoot(refkind);

	if (isroot) {
		ASSERT(referrer_class_tag == 0, "");
		ASSERT(referrer_tag_ptr == NULL, "");

		if (refkind == JVMTI_HEAP_REFERENCE_STACK_LOCAL) {
			ASSERT(refinfo != NULL, "");

			const jvmtiHeapReferenceInfoStackLocal* label =
					&refinfo->stack_local;

			PrintStamp(args->out, label->thread_tag);

			fprintf(args->out, ":%ld:%d:%ld:%d", label->thread_id, label->depth,
					label->location, label->slot);
		} else if (refkind == JVMTI_HEAP_REFERENCE_JNI_LOCAL) {
			ASSERT(refinfo != NULL, "");
			const jvmtiHeapReferenceInfoJniLocal* label = &refinfo->jni_local;

			PrintStamp(args->out, label->thread_tag);

			fprintf(args->out, ":%ld:%d", label->thread_id, label->depth);
		} else {
			ASSERT(refinfo == NULL, "");

			if (refkind == JVMTI_HEAP_REFERENCE_SYSTEM_CLASS) {
				CheckJavaLangClassStamp(args, tag, class_tag);
			}
		}
	} else {
		ASSERT(referrer_class_tag == 0L || StampIsClass(referrer_class_tag),
				"%ld", referrer_class_tag);
		ASSERT(referrer_tag_ptr != NULL, "");

		jlong referrer_tag = *referrer_tag_ptr;

		if (refkind == JVMTI_HEAP_REFERENCE_FIELD
				|| refkind == JVMTI_HEAP_REFERENCE_STATIC_FIELD) {
			fprintf(args->out, ":%4d", refinfo->field.index);

			if (refkind == JVMTI_HEAP_REFERENCE_STATIC_FIELD) {
				CheckJavaLangClassStamp(args, referrer_tag, referrer_class_tag);
			}
		} else if (refkind == JVMTI_HEAP_REFERENCE_ARRAY_ELEMENT) {
			fprintf(args->out, ":%4d", refinfo->array.index);
		} else if (refkind == JVMTI_HEAP_REFERENCE_CONSTANT_POOL) {
			fprintf(args->out, ":%4d", refinfo->constant_pool.index);

			CheckJavaLangClassStamp(args, referrer_tag, referrer_class_tag);
		} else {
			ASSERT(refinfo == NULL, "");

			if (refkind == JVMTI_HEAP_REFERENCE_CLASS) {
				ASSERT(referrer_class_tag == tag, "%ld != %ld",
						referrer_class_tag, tag);
				CheckJavaLangClassStamp(args, tag, class_tag);
			} else if (refkind == JVMTI_HEAP_REFERENCE_SUPERCLASS
					|| refkind == JVMTI_HEAP_REFERENCE_INTERFACE) {
				CheckJavaLangClassStamp(args, referrer_tag, referrer_class_tag);
				CheckJavaLangClassStamp(args, tag, class_tag);
			}
		}

		PrintStamp(args->out, referrer_tag);
		PrintStamp(args->out, referrer_class_tag);
	}

	PrintStamp(args->out, tag);
	PrintStamp(args->out, class_tag);

	fprintf(args->out, ":%d:%ld\n", length, size);

	return JVMTI_VISIT_OBJECTS;
}

void NextHeapRequest(jvmtiEnv* jvmti) {
	jvmtiHeapCallbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.heap_iteration_callback = &heap_iteration_callback;
	callbacks.heap_reference_callback =
			(jvmtiHeapReferenceCallback) &HeapReferenceCallback;

	FollowReferencesArgs args;

//	char filename[1024];
//
//	sprintf(filename, "./db/fr.%08d.log", sample);

	args.out = NULL;
	//args.init = false;
//	args.javaLangClassTag = 0;
//	args.sample = sample;

//const int size = 1024 * 1024;
//args.frbuf = malloc(size);
//check_std_error(setvbuf(args.out, args.frbuf, _IOFBF, size), "setvbuf");

	FrFollowReferences(jvmti, 0, NULL, NULL, &callbacks, &args);
	_TLOG("ENDSAMPLE:%d", args.sample);

	ASSERT(args.out != NULL, "Invalid file to close");
	fclose(args.out);
	//free(args.frbuf);

	/*
	 sprintf(args.filename, "logs/sh-tag-it-%04d.log", sample);
	 args.out = fopen(args.filename, "w");
	 error = (*jvmti_env)->IterateThroughHeap(
	 jvmti_env,		// jvmtiEnv* env
	 0,				// jint heap_filter
	 NULL,			// jclass klass
	 &callbacks,		// const jvmtiHeapCallbacks* callbacks
	 &args			// const void* user_data
	 );
	 check_jvmti_error(jvmti_env, error, "Unable to FollowReferences in HeapRequest.");
	 fclose(args.out);
	 */
}
