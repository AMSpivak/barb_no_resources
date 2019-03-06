#include <stdexcept>
#include "gl_resources_manager.h"

void GLResourcesManager::Clean()
{
}

static GLResourcesManager * main_resource_manager = nullptr;

GLResourcesManager * GetResourceManager()
{
    if(main_resource_manager == nullptr)
    {
        throw std::out_of_range("No recource manager defined!");
    }
    else
    return main_resource_manager;
}

void SetResourceManager(GLResourcesManager * resource_manager)
{
    main_resource_manager = resource_manager;
}