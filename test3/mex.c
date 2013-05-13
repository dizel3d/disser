#include <math.h>
#include <matrix.h>
#include <mex.h>

static const char* fieldnames[] = {"res", "real", "user", "sys"};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (!(nrhs == 5 && mxIsChar(prhs[0]) && mxIsNumeric(prhs[1]) && mxIsNumeric(prhs[2]) && mxIsNumeric(prhs[3]) && mxIsNumeric(prhs[4]))) {
		mexErrMsgTxt("Invalid parameters");
	}

	char thread_class[64];
	mxGetString(prhs[0], thread_class, sizeof(thread_class));

	int test_number = mxGetScalar(prhs[1]);
	int test_time = mxGetScalar(prhs[2]);
	int thread_num = mxGetScalar(prhs[3]);
	int process_num = mxGetScalar(prhs[4]);

	char cmd[256];
	sprintf(cmd, "(time -p ./%s %i %i %i %i) 2>&1", thread_class, test_number, test_time, thread_num, process_num);

	FILE* f;
	if ((f = popen(cmd, "r")) == NULL) {
		mexErrMsgIdAndTxt("Exec:Error", "Cannot invoke '%s'", cmd);
		return;
	}

	char output[512];
	int offset = 0;
	int n;
	while ((n = fread(output + offset, 1, sizeof(output), f)) > 0) {
		offset += n;
	}

	pclose(f);

	if (nlhs) {
		double res, real, user, sys;
		sscanf(output, "%lf\nreal %lf\nuser %lf\nsys %lf", &res, &real, &user, &sys);
		mxArray* result = mxCreateStructMatrix(1, 1, 4, fieldnames);
		mxSetField(result, 0, "res", mxCreateDoubleScalar(res));
		mxSetField(result, 0, "real", mxCreateDoubleScalar(real));
		mxSetField(result, 0, "user", mxCreateDoubleScalar(user));
		mxSetField(result, 0, "sys", mxCreateDoubleScalar(sys));
		plhs[0] = result;
	} else {
		mexPrintf("Result of '%s':\n%s\n", cmd, output);
	}
}
