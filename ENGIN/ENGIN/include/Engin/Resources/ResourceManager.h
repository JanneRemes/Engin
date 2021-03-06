#pragma once
#include <unordered_map>
#include <iostream>
#include "Engin\Resources\Resource.h"

namespace Engin
{
	namespace Resources
	{
		class ResourceManager
		{
		public:
			ResourceManager() {};
			~ResourceManager();

			static ResourceManager& getInstance()
			{
				static ResourceManager instance;
				return instance;
			}

			ResourceManager(const ResourceManager&) = delete;
			void operator=(const ResourceManager&) = delete;

			template <class T>
			T* load(const std::string &filename)
			{
				if (filename.empty())
				{
					std::cout << "Filename cannot be null! Error loading resource." << std::endl;
					return nullptr;
				}

				std::unordered_map<std::string, Resource*>::iterator it;
				it = resourceMap.find(filename);

				if (it != resourceMap.end())
				{
					(*it).second->incReferences();
					return static_cast<T*>((*it).second);
				}

				T *resource = new T(filename);
				resource->incReferences();

				resourceMap.insert(std::pair<std::string, Resource*>(filename, resource));
				return static_cast<T*>(resource);
			};

			bool unload(const std::string &filename);

			void printResources();

		private:
			std::unordered_map< std::string, Resource*> resourceMap;
			void releaseAll();
		};
	}
}