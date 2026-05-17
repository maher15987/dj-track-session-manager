#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
   : decks{nullptr, nullptr}
    , active_deck(0)
    , auto_sync(false)
    , bpm_tolerance(0)
{
    std::cerr << "[MixingEngineService] Initialized with 2 empty decks.\n";
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    // Your implementation here
    std::cerr << "[MixingEngineService] Cleaning up decks...\n";
    if (decks[0] != nullptr){
        delete decks[0];
        decks[0] = nullptr;
    }
    if (decks[1] != nullptr){
        delete decks[1];
        decks[1] = nullptr;
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
    std::cerr << "\n=== Loading Track to Deck ===\n";
    // first track
    if (decks[0] == nullptr && decks[1] == nullptr) {
        PointerWrapper<AudioTrack> cloned = track.clone();
        if (!cloned) {
            std::cerr << "[ERROR] Track \"" << track.get_title()
                      << "\" failed to clone\n";
            return -1;
        }
        decks[0] = cloned.release();
        decks[0]->load();
        decks[0]->analyze_beatgrid();
        active_deck = 0;
        std::cerr << "[Load Complete] '" 
                  << decks[0]->get_title()
                  << "' is now loaded on deck 0\n";
        return 0;
    }

    // SUBSEQUENT TRACKS
    PointerWrapper<AudioTrack> cloned = track.clone();
    if (!cloned) {
        std::cerr << "[ERROR] Track \"" << track.get_title()
                  << "\" failed to clone\n";
        return -1;
    }
    int previous = active_deck;
    int target = 1 - active_deck;
    // (e) 
    std::cerr << "[Deck Switch] Target deck: " << target << "\n";
    // (f) 
    if (decks[target] != nullptr) {
        delete decks[target];
        decks[target] = nullptr;
    }
    // (g) 
    cloned->load();
    cloned->analyze_beatgrid();
    // (h) 
    if (auto_sync && decks[previous] != nullptr) {
        int bpm_old = decks[previous]->get_bpm();
        int bpm_new = cloned->get_bpm();
        int diff = std::abs(bpm_new - bpm_old);
        if (diff > bpm_tolerance) {
            std::cerr << "[BPM Sync] Difference (" << diff 
                      << ") exceeds tolerance (" 
                      << bpm_tolerance << ")\n";
            sync_bpm(cloned);
        }
    }
    // (i) 
    decks[target] = cloned.release();
    std::cerr << "[Load Complete] '" 
              << decks[target]->get_title()
              << "' is now loaded on deck " << target << "\n";
    // (k) 
    active_deck = target;
    // (l) 
    std::cerr << "[Active Deck] Switched to deck " 
              << active_deck << "\n";
    // (m) 
    return target;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(!decks[active_deck]){
        return false;
    }
    if(!track){
        return false;
    }
    int bpm1=decks[active_deck]->get_bpm();
    int bpm2=track->get_bpm();
    int diff = std::abs(bpm1 - bpm2);
    if(diff<=bpm_tolerance){
        return true;
    }
    else 
        return false;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here

    if (decks[active_deck] == nullptr)
        return;
    if (!track)
        return;

    int bpm_old = decks[active_deck]->get_bpm();
    int bpm_original = track->get_bpm();
    int bpm_average = (bpm_old + bpm_original) / 2;

    std::cerr << "[Sync BPM] Syncing BPM from "
              << bpm_original << " to " << bpm_average << "\n";
    track->set_bpm(bpm_average);
}
