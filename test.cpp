
			    string filePath;
    if (route.getAllowListing() == true)
        filePath = server.getRoot() + route.getPath();
    else
        filePath = request->getPath();
