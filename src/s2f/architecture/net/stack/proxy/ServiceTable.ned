package s2f.architecture.net.stack.proxy;

// Standalone table that keeps the service entries
// 
// @author Ulysses de Aguilar Gudmundsson
// @date 2024-01-12
simple ServiceTable {
    parameters:
    	@class(ServiceTable);
        string domain = default("localdomain");
}