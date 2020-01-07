#include "animation_sequence.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include <sstream>
#include <array>

#include "loader.h"

std::vector<std::pair<std::string,AnimationCommand>> commands_list = {
                                                        {"", AnimationCommand::kNone},
                                                        {"message", AnimationCommand::kMessage},
                                                        {"strike", AnimationCommand::kStrike},
                                                        {"strike_forward", AnimationCommand::kStrikeForward},
                                                        {"strike_left", AnimationCommand::kStrikeRight},
                                                        {"strike_right", AnimationCommand::kStrikeLeft},
                                                        {"move", AnimationCommand::kMove},
                                                        {"move_fast", AnimationCommand::kFastMove},
                                                        {"rotate", AnimationCommand::kRotate},
                                                        {"use", AnimationCommand::kUse},
                                                        {"executed", AnimationCommand::kExecuted},
                                                        {"damage", AnimationCommand::kDamaged},
                                                        {"stance", AnimationCommand::kStance},
                                                        {"sound", AnimationCommand::kSound},
                                                        {"step_back", AnimationCommand::kStepBack},
                                                        {"step_right", AnimationCommand::kStepRight},
                                                        {"step_left", AnimationCommand::kStepLeft},
                                                        {"guard", AnimationCommand::kGuard},
                                                        {"strike_block", AnimationCommand::kStrikeBlocked},
                                                        {"rotate", AnimationCommand::kRotate}

                                                    };



AnimationCommand GetCommand(const std::string & name)
{
    auto find_from = commands_list.begin();
    auto find_to = commands_list.end();
    auto val = std::find_if(find_from,find_to,[name](std::pair<std::string,AnimationCommand> b){return b.first == name;});
    if(val == find_to)
        return AnimationCommand::kNone;
    return val->second;
}

const std::string &GetCommandName(const AnimationCommand command)
{
    auto find_from = commands_list.begin();
    auto find_to = commands_list.end();
    auto val = std::find_if(find_from,find_to,[command](std::pair<std::string,AnimationCommand> b){return b.second == command;});
    if(val == find_to)
        return commands_list.begin()->first;
    return val->first;
}

std::pair<AnimationCommand,std::string> ParseCommand(const std::string &command)
{
    if(command =="")
    {
        return std::make_pair(AnimationCommand::kNone,"");
    }
    std::stringstream command_stream(command);
    AnimationCommand cmd_id = GetCommand(LoaderUtility::GetFromStream<std::string>(command_stream));
    std::string params;
    std::string dummy;
    
    std::getline(std::getline(command_stream,dummy,' '), params);

    return std::make_pair(cmd_id,params);
}



std::string CommandToStream(std::pair<AnimationCommand,std::string> value)
{
    
    std::stringstream command_stream;
    try
    {
        command_stream << GetCommandName(value.first);
        if(value.second != "") command_stream<<" "<<value.second;
        return command_stream.str();
    }
    catch(const std::out_of_range& exp)
    {
        std::cout << "<error------->!\n";
        return "";
    }
}

std::istream& operator>> ( std::istream& is, AnimationSequence & value)
{
    std::string skip;
    int tmp_block = 0;
	is>>value.start_frame>>value.end_frame>>value.m_focus>>tmp_block>>value.m_no_rotation>>value.m_loop>>value.m_jump;
    value.m_block = tmp_block == 0 ? DamageReaction::Damage : DamageReaction::Block;
    if(value.m_jump)
        is>>value.m_target_sequence;
    std::string tmp;
    //is >>std::quoted(tmp);
    std::getline(std::getline(is, skip, '"'), tmp, '"') ;

    value.m_start_message = ParseCommand(tmp);
    // std::cout << "<m_start_message------->"<<tmp<<"!\n";
    //is>>std::quoted(tmp);
    std::getline(std::getline(is, skip, '"'), tmp, '"') ;

    value.m_frame_message = ParseCommand(tmp);
    // std::cout << "<m_frame_message------->"<<tmp<<"!\n";
    //is>>std::quoted(tmp);
    std::getline(std::getline(is, skip, '"'), tmp, '"') ;
    value.m_end_message = ParseCommand(tmp);
    // std::cout << "<m_end_message------->"<<tmp<<"!\n";
    // std::cout << "<commands-------> \n";
    tmp = "";
    while(!is.eof())
    {
        //is >>std::quoted(tmp);
        std::getline(std::getline(is, skip, '"'), tmp, '"') ;
        // std::cout << "<skip><" << skip <<"> \n";
        if(skip == "" && tmp == "") return is;

        if(tmp!="")
        {
            value.jumps.insert( ParseCommand(tmp));
            // std::cout << "<command><" << tmp <<"> \n";
        }
        tmp="";

    }
    // std::cout << "<commands-------> \n";

    return is;
    //std::cout<<"animation "<<value;
}

std::ostream& operator << ( std::ostream& os, const AnimationSequence & value)
{
    int tmp_block = value.m_block == DamageReaction::Damage ? 0 : 1;
    os<<value.start_frame<<" "<<value.end_frame<<" "<<value.m_focus<<tmp_block<<value.m_no_rotation<<value.m_loop<<" "<<value.m_jump;
    if(value.m_jump) 
        os<<" "<<value.m_target_sequence;
        os<<" "<<std::quoted(CommandToStream(value.m_start_message))<<" "
        <<std::quoted(CommandToStream(value.m_frame_message))<<" "
        <<std::quoted(CommandToStream(value.m_end_message));
        for(auto jump : value.jumps)
        {
            os<<" "<<std::quoted(CommandToStream(jump));
        }

        //os<<"\n";
	return os;
}