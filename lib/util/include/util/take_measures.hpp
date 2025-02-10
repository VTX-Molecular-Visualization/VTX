#ifndef TAKE_MEASURES_HPP
#define TAKE_MEASURES_HPP

#include <iostream>
#include <string>
#include <memory>
#include "filesystem.hpp"

using namespace VTX::Util;
class TakeMeasures {
    public :
        TakeMeasures(const int p_measureID, 
                    const std::string & p_filePath,
                     const int p_measureTime = 1) : _measureID(p_measureID), 
                                                    _filePath(p_filePath),
                                                    _measureTime(p_measureTime) 

                                                     {};
        void start(){
            _isStarted = true;
            Filesystem::writeLineToFile(_filePath, _measureName + "\n");

        }   ;                                          
        void measure(float deltaTime, VTX::Renderer::InstructionsDurationRanges const & p_durations){
            //TODO cette methode est giga fausse
            _updateDuration(p_durations);

            //write measure name into file
            if(_currentDuration<0.0000001f ){
                
                Filesystem::writeLineToFile(_filePath, _duration.name + "\n");
            }

            //write measure into file
            Filesystem::writeLineToFile(_filePath,std::to_string(_duration.duration));
            _updateTime(deltaTime);

            //stop the measure after a certain time
            if (_currentDuration > _measureTime ){
                _isStarted = false;
                _currentDuration = 0.f;
            }
        };

        bool isStarted() const {return _isStarted;}
    private :
        std::string _measureName;
        const int _measureID;
        std::shared_ptr<VTX::Renderer::InstructionsDurationRanges> _durations ; //durations to measure
        std::string _filePath; //where to write the measureS
        float _measureTime; //how long the measure should last
        VTX::Renderer::InstructionsDurationRange _duration;
        float _currentDuration = 0.f;

        bool _isStarted = false;

        inline void _updateTime(float deltaTime){
            if(_isStarted){
                _currentDuration += deltaTime;
            }
        }  

        void _updateDuration(VTX::Renderer::InstructionsDurationRanges const & p_durations){
            if (_isStarted)
                _duration = p_durations[_measureID];
        }

    
};
#endif