#include "glvideo.h"
#include <iostream>

using namespace std;

int main() {
    string filename = "/Users/ian/Desktop/Bed00086523_V2-0002.mov";

    glvideo::Player player( filename );

    cout << "Format: " << player.getFormat() << endl;
    cout << "Duration (seconds): " << player.getDuration() << endl;
    cout << "Number of tracks: " << player.getNumTracks() << endl;
    for ( int i = 0; i < player.getNumTracks(); ++i ) {
        cout << "\tTrack " << i << " type: " << player.getTrackDescription( i ) << endl;
    }
    return 0;
}