DROP TABLE IF EXISTS "User" CASCADE;
CREATE TABLE "User" (
                        "username" VARCHAR(255) NOT NULL,
                        "password" VARCHAR(255) NOT NULL,
                        "status" VARCHAR(1) DEFAULT '0' CHECK ("status" IN ('0', '1')),
                        PRIMARY KEY ("username")
);

DROP TABLE IF EXISTS "Friend" CASCADE;
CREATE TABLE "Friend" (
                        "username" VARCHAR(255) NOT NULL,
                        "friend_name" VARCHAR(255) NOT NULL,
                        "status" VARCHAR(10) NOT NULL CHECK ("status" IN ('pending', 'accepted')),
                        PRIMARY KEY ("username", "friend_name"),
                        FOREIGN KEY ("username") REFERENCES "User" ("username"),
                        FOREIGN KEY ("friend_name") REFERENCES "User" ("username")
);

DROP TABLE IF EXISTS "MessageLog" CASCADE;
CREATE TABLE "MessageLog" (
                              messageId SERIAL PRIMARY KEY,
                              sender VARCHAR(255),
                              receiver VARCHAR(255),
                              "content" TEXT,
                              sentTime VARCHAR(255),
                              FOREIGN KEY ("sender") REFERENCES "User" ("username"),
                              FOREIGN KEY ("receiver") REFERENCES "User" ("username")
);

DROP TABLE IF EXISTS "Group" CASCADE;
CREATE TABLE "Group" (
    "groupId" SERIAL PRIMARY KEY,
    "groupName" VARCHAR(255) NOT NULL UNIQUE,
    "creator" VARCHAR(255) NOT NULL REFERENCES "User" ("username"),
    "createdAt" TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

DROP TABLE IF EXISTS "GroupMember" CASCADE;
CREATE TABLE "GroupMember" (
    "groupId" INT NOT NULL REFERENCES "Group" ("groupId") ON DELETE CASCADE,
    "username" VARCHAR(255) NOT NULL REFERENCES "User" ("username") ON DELETE CASCADE,
    "role" VARCHAR(20) DEFAULT 'member' CHECK ("role" IN ('member','owner')),
    "joinedAt" TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY ("groupId","username")
);

DROP TABLE IF EXISTS "GroupMessage" CASCADE;
CREATE TABLE "GroupMessage" (
    "id" SERIAL PRIMARY KEY,
    "groupId" INT NOT NULL REFERENCES "Group" ("groupId") ON DELETE CASCADE,
    "sender" VARCHAR(255) NOT NULL REFERENCES "User" ("username"),
    "content" TEXT,
    "sentTime" TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    "delivered" BOOLEAN DEFAULT FALSE
);

--- Insert sample data into User table
INSERT INTO "User" ("username", "password")
VALUES
    ('user1', 'password1'),
    ('user2', 'password2'),
    ('user3', 'password3'),
    ('user4', 'password4'),
    ('user5', 'password5'),
    ('user6', 'password6'),
    ('user7', 'password7'),
    ('user8', 'password8'),
    ('user9', 'password9'),
    ('user10', 'password10');

INSERT INTO "Friend" ("username", "friend_name", "status")
VALUES
    ('user1', 'user2', 'accepted'),
    ('user2', 'user1', 'accepted'),
    ('user1', 'user4', 'accepted'),
    ('user4', 'user1', 'accepted'),
    ('user3', 'user6', 'accepted'),
    ('user6', 'user3', 'accepted'),
    ('user4', 'user8', 'accepted'),
    ('user8', 'user4', 'accepted'),
    ('user1', 'user10', 'accepted'),
    ('user10', 'user1', 'accepted');