#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    // Your implementation here 
    if (cache.contains(track.get_title())) {
        cache.get(track.get_title());
        return 1; 
    }

    
    PointerWrapper<AudioTrack> cloned = track.clone();
     // Clone failed return error.
    if (!cloned) {
        std::cerr << "eror track failed to clone " << track.get_title() << "\n";
        return 0;
    }
    //Prepare track before caching.
    cloned->load();
    cloned->analyze_beatgrid();

    bool eviction = cache.put(std::move(cloned));

    if (eviction)
        return -1;     
    else
        return 0;
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    // Your implementation here

        AudioTrack* track = cache.get(track_title);
        if(track){
            return track;
        }
        else{
        return nullptr;
        }
}
