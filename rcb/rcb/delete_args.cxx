#include <string>
#include <vector>
#include <print>

#include "delete_args.hxx"
#include "delete.hxx"
#include "common/globals.hxx"

namespace rcb{

DeleteArgs::DeleteArgs(std::vector<std::string>& args)
{
	run(args);
}

void DeleteArgs::run(std::vector<std::string>& args)
{
#ifndef NDEBUG
	for(auto it : args)
	{
		std::println("In delete_args:{}", it);
	}
#endif

	if(args.size() <= 0)
	{
		std::println("{0} delete: missing operand\ntry \'{0} --help\' for more information", g_progName);
		return;
	}

	for(auto i { 0UL }; i < args.size(); i++)
	{
	
		if (std::string(args.at(i)).substr(0, 2) == "--" || std::string(args.at(i)).substr(0, 1) == "-")
		{
			//Add Conditional check for '-'. Reads stdin/stdout. E.g cat -

			if(std::string(args.at(i)) == "--help" || std::string(args.at(i)) == "-h")
			{
				std::print("{}", m_deleteHelpMsg);
				return;
			} 
			else if(std::string(args.at(i)) == "--")
			{
				//Remove -- before passing arguments to delete cmd.
				m_erasePositions.push_back(i);
				break;
			} 
			else if(std::string(args.at(i)) == "--verbose" || std::string(args.at(i)) == "-v")
			{
				m_dOpt.verboseOption = true;
				m_erasePositions.push_back(i);
			} 
			else if(std::string(args.at(i)) == "--force" || std::string(args.at(i)) == "-f")
			{
				//TODO: Use for critical files/directories such as .rcb/, /*,
				//Just for '/' Add a 'special' message as default without --force.
				//Make the prompt red to get their attention.
				//Add a \a sound as-well for the non-force in case they are colour blind.
				
				//unused
				m_dOpt.forceOption = true;
				m_erasePositions.push_back(i);
			}
			else if(std::string(args.at(i)) == "--silent" || std::string(args.at(i)) == "-s")
			{
				m_dOpt.silentOption = true;
				m_erasePositions.push_back(i);
			}
			else if(std::string(args.at(i)) == "--no-directorysize")
			{
				m_dOpt.noDirSizeOption = true;
				m_erasePositions.push_back(i);
			} 
			else
			{
				std::println("unknown option: {0}", args.at(i));
				return; //Prevent continuation
			}
		}
		else
		{
			//Unused
		}
	}

	//Needs to be removed in reverse to prevent index shift back.
	for (auto it = m_erasePositions.rbegin(); it != m_erasePositions.rend(); ++it) 
	{
		args.erase(args.begin() + *it);
	}

	Delete{args, m_dOpt};
}

} // namespace rcb
