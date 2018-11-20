#include <jni.h>
//#include "eventprocessor.hpp"
//#include "eventprocessor2L.hpp"
#include "eventprocessor1L.hpp"

extern "C" {

//TODO
//use tool to link wrapper to new namespace packaging
JNIEXPORT void JNICALL Java_com_example_chronocam_atis_eventprocessor_1moduleJNI_Eventprocessor_1init(JNIEnv *env, jclass jcls, jlong jniCPtr, jobject jarg1_, jstring l1ProtoPath_, jstring l2ProtoPath_, jstring gestureSigPath_) {

        __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor init L1 ", "IM HEREEEE");

        EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
		const char *l1ProtoPath = env->GetStringUTFChars(l1ProtoPath_, 0);
		const char *l2ProtoPath = env->GetStringUTFChars(l2ProtoPath_, 0);
		const char *gestureSigPath = env->GetStringUTFChars(gestureSigPath_, 0);

		std::string stdL1ProtoPath(l1ProtoPath, 100);
		stdL1ProtoPath.erase(std::find(stdL1ProtoPath.begin(), stdL1ProtoPath.end(), '\0'), stdL1ProtoPath.end());
		std::string stdL2ProtoPath(l2ProtoPath, 100);
		stdL2ProtoPath.erase(std::find(stdL2ProtoPath.begin(), stdL2ProtoPath.end(), '\0'), stdL2ProtoPath.end());
		std::string stdGestureSigPath(gestureSigPath, 100);
		stdGestureSigPath.erase(std::find( stdGestureSigPath.begin(), stdGestureSigPath.end(), '\0'), stdGestureSigPath.end());
		(eventProcessor)->init(stdL1ProtoPath, stdL2ProtoPath, stdGestureSigPath);
		env->ReleaseStringUTFChars(l1ProtoPath_, l1ProtoPath);
		env->ReleaseStringUTFChars(l2ProtoPath_, l2ProtoPath);
		env->ReleaseStringUTFChars(gestureSigPath_, gestureSigPath);
	}

	JNIEXPORT void JNICALL Java_com_example_chronocam_atis_eventprocessor_1moduleJNI_Eventprocessor_1set_1camera_1data(JNIEnv *env, jclass jcls, jlong jniCPtr, jobject jarg1_, jbyteArray camera_data_, jlong camera_data_length) {
		EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
		jbyte *camera_data = env->GetByteArrayElements(camera_data_, 0);

		(eventProcessor)->setCameraData((unsigned char *) camera_data, (unsigned long) camera_data_length);

		env->ReleaseByteArrayElements(camera_data_, camera_data, 0);
	}

	JNIEXPORT jint JNICALL Java_com_example_chronocam_atis_eventprocessor_1moduleJNI_Eventprocessor_1predict(JNIEnv *env, jclass jcls, jlong jniCPtr) {
		jint jresult = 0;
		EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
		jresult = (jint) (eventProcessor)->predict();
		return jresult;
	}

	JNIEXPORT jlong JNICALL Java_com_example_chronocam_atis_eventprocessor_1moduleJNI_new_1Eventprocessor(JNIEnv *env, jclass jcls) {
		jlong jresult = 0;
		EventProcessor *result = 0;
		result = (EventProcessor *) new EventProcessor();
		*(EventProcessor **) &jresult = result;
		return jresult;
	}

	JNIEXPORT void JNICALL Java_com_example_chronocam_atis_eventprocessor_1moduleJNI_delete_1Eventprocessor(JNIEnv *env, jclass jcls, jlong jniCPtr) {
		EventProcessor *eventprocessor = *(EventProcessor **) &jniCPtr ;
		(eventprocessor)->cleanUp();
		delete eventprocessor;
	}
}