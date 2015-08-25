﻿#summary Static vs. Dynamic linking
#labels Phase-Implementation,Phase-Design

# Introduction #
Static versus dynamic linking of modules is the main concept of Mojito modules.

# Overview #
From a Mojito point of view, a module is simply a struct of function pointers plus some internal data, like the module name or the module category.
The module interface is unique for each module, giving the core the possibility to load and use dynamically each module in a standardized way.

# Architecture #
Here it is a simple schema.

![http://mojito-webserver.googlecode.com/svn/wiki/design_attachments/static_vs_dynamic.png](http://mojito-webserver.googlecode.com/svn/wiki/design_attachments/static_vs_dynamic.png)

.dia file here: http://mojito-webserver.googlecode.com/svn/wiki/design_attachments/static_vs_dynamic.dia

  * Linking statically you insert into the final object file only the required module of that given type.
  * Linking dynamically you insert into the final object file only the generic dlloader, while building each other module separately.

**NOTE**: For each module-set you can choose the type of linkage. So you can have a static linked logger and a bunch of caching modules to choose from at runtime.

# Module interface #
![http://mojito-webserver.googlecode.com/svn/wiki/design_attachments/module_interface.png](http://mojito-webserver.googlecode.com/svn/wiki/design_attachments/module_interface.png)

http://mojito-webserver.googlecode.com/svn/wiki/design_attachments/module_interface.dia