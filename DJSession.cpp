#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */

 DJLibraryService::~DJLibraryService() {
    for (AudioTrack* t : library) {
        delete t;
    }
    library.clear();
}

void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    //Todo: Implement buildLibrary method
    library.clear(); 
    for(const auto& trackInfo : library_tracks) {
        AudioTrack* track = nullptr;
        if (trackInfo.type == "MP3") {
            bool hasTags = (trackInfo.extra_param2 != 0);
            track = new MP3Track(trackInfo.title,trackInfo.artists,trackInfo.duration_seconds,trackInfo.bpm,trackInfo.extra_param1,hasTags);
            std::cout << "MP3Track created: " << trackInfo.extra_param1 << " kbps\n";
        }
        else if (trackInfo.type == "WAV") {
            track = new WAVTrack(trackInfo.title,trackInfo.artists,trackInfo.duration_seconds,trackInfo.bpm,trackInfo.extra_param1,trackInfo.extra_param2);
            std::cout << "WAVTrack created: " 
                      << trackInfo.extra_param1 << "Hz/" 
                      << trackInfo.extra_param2 << "bit\n";
        }
        
        library.push_back(track);
    }

    std::cout << "[INFO] Track library built: " 
              << library.size() << " tracks loaded\n";
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
    AudioTrack* track= playlist.find_track(track_title);
    if(track!=nullptr){
      return track;
    }

    return nullptr;
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    // For now, add a placeholder to fix the linker error

    std::cout << "[INFO] Loading playlist: " << playlist_name << "\n";
    playlist = Playlist(playlist_name);

    for (int index : track_indices) {
        if (index < 1 || index > (int)library.size()) {
            std::cerr << "[WARNING] Invalid track index: " << index << "\n";
            continue;
        }
        AudioTrack* original_track = library[index - 1];
        PointerWrapper<AudioTrack> clone_ptr(original_track->clone());
        if (clone_ptr.get()==nullptr) {
            std::cerr << "[ERROR] Failed to clone track: " 
                      << original_track->get_title() << "\n";
            continue;
        }
        clone_ptr.get()->load();
        clone_ptr.get()->analyze_beatgrid();
        playlist.add_track(clone_ptr.release());
        std::cout << "Added '"<< original_track->get_title()<< "' to playlist '" << playlist_name << "'\n";
    }
    std::cout << "[INFO] Playlist loaded: "<< playlist_name<< " (" << playlist.get_track_count() << " tracks)\n";
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here


    //Extract titles from playlist track list.
    std::vector<std::string> titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for (AudioTrack* track : tracks) {
        if (track) {
            titles.push_back(track->get_title());
        }
    }
    return titles;
}
