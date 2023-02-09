#include <iostream>
#include <string>

using namespace std;

const int BUFFER_SIZE = 256;

int fast_atoi(const char* str) {
    int val = 0;

    while (*str) {
        if (*str - '0' >= 0) {
            val *= 10;
            val += (*str++ - '0');
        } else {
            break;
        }
    }

    return val;
}

string exec(string cmd) {
    
    char buffer[BUFFER_SIZE];
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);

    if (!pipe) {
        throw runtime_error("popen() failed!");
    }

    while (fgets(buffer, BUFFER_SIZE, pipe.get()) != nullptr) {
        result += buffer;
    }

    return result;
}

int get_frame_count(string video_name) {
    int frame_count = 0;
    string cmd = "ffprobe -v error -select_streams v:0 -count_packets -show_entries stream=nb_read_packets -of csv=p=0 " + video_name;
    

    frame_count = fast_atoi(exec(cmd).c_str());

    return frame_count;
}

int get_duration(string video_name) {
    int duration = 0;
    string cmd = "ffmpeg -i "+ video_name +" 2>&1 | grep \"Duration\"| cut -d ' ' -f 4 | sed s/,// | sed 's@\\..*@@g' | awk '{ split($1, A, \":\"); split(A[3], B, \".\"); print 3600*A[1] + 60*A[2] + B[1] }'";
    
    duration = fast_atoi(exec(cmd).c_str());

    return duration;
}

void get_frames(int frame_count, string video_name) {
    string cmd = "mkdir frames &>/dev/null && ffmpeg -i "+video_name+" -filter:v fps=fps="+to_string(frame_count / get_duration(video_name))+" ./frames/frame_%04d.bmp &>/dev/null";
    exec(cmd);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Error: You must provide a video name!\n" << endl;
        return 1;
    }

    string video_name(argv[1]);

    int frame_count = get_frame_count(video_name);
    int duration = get_duration(video_name);

    cout << "frame_count  = " << frame_count << endl;
    cout << "fps          = " << (double) frame_count / duration << endl;
    cout << "duration     = " << get_duration(video_name) << endl;;

    clock_t start, end;
    double cpu_time;

    start = clock();
    get_frames(frame_count, video_name);
    end = clock();

    cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    cout << "frame_p_time = " << cpu_time * 10000 << "s" << endl;

    return 0;
}