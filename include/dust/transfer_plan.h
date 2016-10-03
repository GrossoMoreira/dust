#ifndef _DUST_TRANSFER_PLAN_
#define _DUST_TRANSFER_PLAN_

#include "generic_hash.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace dust
{
	template <typename NodeId> class transfer_plan
	{

		public:

			enum transfer_type
			{
				SPLIT,
				JOIN,
				BACKUP,
				NEGLECT
			};

		private:

			struct interval
			{
				hash lower_bound;
				hash upper_bound;

				std::string str()
				{
						std::stringstream ss;
						ss << lower_bound.str() << " ~ " << upper_bound.str();
						return ss.str();
				}
			};

			struct transfer
			{
				NodeId from;
				NodeId to;
				transfer_type type;
				interval data;

				std::string str()
				{
					std::stringstream ss;
					switch(type)
					{
						case SPLIT:
							ss << "SPLI";
							break;
						case JOIN:
							ss << "JOIN";
							break;
						case BACKUP:
							ss << "BACK";
							break;
						case NEGLECT:
							ss << "NEGL";
							break;
					}

					ss << " " << from << " -> " << to << ": " << data.str();
					return ss.str();
				}
			};

			std::vector<transfer> queue;

		public:

			void plan(hash lower_bound, hash upper_bound, NodeId from, NodeId to, transfer_type type)
			{
				queue.push_back({ from, to, type, { lower_bound, upper_bound} });
			}

			void merge(transfer_plan&& other)
			{
				std::move(other.queue.begin(), other.queue.end(), std::back_inserter(queue));
			}

			void dump()
			{
				std::cout << "Transfer plan:\n";
				for(auto& t : queue)
					std::cout << t.str() << std::endl;
			}
	};
}

#endif // _DUST_TRANSFER_PLAN_
