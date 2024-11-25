DROP TABLE appuser, file, tag, filetag, edition, favorite, pool, poolfile;

CREATE TABLE appuser(
	username VARCHAR(50),
	userID SERIAL,
	PRIMARY KEY(userID),
	UNIQUE(username)
);

CREATE TABLE file(
	fileID SERIAL,
	filename VARCHAR(255),
	createdby INT,
	createdon TIMESTAMP,
	type VARCHAR(50),
	size INT,
	views INT DEFAULT 0,
	PRIMARY KEY(fileID),
	FOREIGN KEY(createdby) REFERENCES appuser,
	UNIQUE(filename, createdon)
);

CREATE TABLE tag(
	tagname VARCHAR(50),
	tagID SERIAL,
	PRIMARY KEY(tagID),
	UNIQUE(tagname)
);

CREATE TABLE filetag(
	filetagID SERIAL,
	fileID INT,
	tagID INT,
	PRIMARY KEY(filetagID),
	UNIQUE(fileID, tagID),
	FOREIGN KEY(fileID) REFERENCES file,
	FOREIGN KEY(tagID) REFERENCES tag
);

CREATE TABLE edition(
	editionID SERIAL,
	fileID INT,
	userID INT,
	editiondate TIMESTAMP,
	PRIMARY KEY(editionID),
	UNIQUE(fileID, userID, editiondate),
	FOREIGN KEY(fileID) REFERENCES file,
	FOREIGN KEY(userID) REFERENCES appuser
);

CREATE TABLE favorite(
	favoriteID SERIAL,
	fileID INT, 
	userID INT,
	PRIMARY KEY(favoriteID),
	UNIQUE(fileID, userID),
	FOREIGN KEY(fileID) REFERENCES file,
	FOREIGN KEY(userID) REFERENCES appuser
);

CREATE TABLE pool(
	poolID SERIAL,
	poolname VARCHAR(255),
	PRIMARY KEY(poolID),
	UNIQUE(poolname)
);

CREATE TABLE poolfile(
	poolfileID SERIAL,
	poolID INT,
	fileID INT,
	PRIMARY KEY(poolfileID),
	FOREIGN KEY(poolID) REFERENCES pool,
	FOREIGN KEY(fileID) REFERENCES file,
	UNIQUE(poolID, fileID)
);
