#! usr/bin/env python

import pygame, math, random, copy
from pygame.locals import *

class Options:
    def __init__(self):
        class Game:
            def __init__(self):
                self.powerNames = ["Intimidation","Persuasion","Bribery","Virgins"];
                self.powerShortNames = ["I","P","B","V"];
                self.powerColors = [(255, 0, 0), (0,255,255),(0,255,0),(255,255,0)];
                self.followerNames = ["Neophyte","Adept","Priest"];
                self.powerConversionCost = [2,2,2,1];
                self.willPowerCost = 2;
                self.lineColors = [(0x55, 0xdd, 0x55),(0x27, 0x27, 0xd7),(0xe0, 0x52, 0xca),(0xd8, 0xe1, 0x51)];
                self.nodeColors = [(0, 0x55, 0),(1, 9, 0x55),(0x56, 0, 0x53),(0x50, 0x50, 0)];
                self.cultColors = [(0, 0xb4, 0),(0x2f, 0x43, 0xfd),(0xb4, 0, 0xae),(0xb4, 0xae, 0)];
                self.version = "v3";
                self.followerLevels = 3;
                self.numPowers = 3;
                self.numCults = 4;
                self.numSummonVirgins = 9;
                self.nodesCount = 100;
                self.upgradeCost = 3;
                self.isDebug = False;
                self.debugTime = False;
                self.debugVis = False;
                self.debugNear = False;
                self.debugAI = False;
                self.mapVisible = False;
        self.Game = Game()

        class UI:
            def __init__(self):
                self.winWidth = 1024;
                self.winHeight = 600;
                self.mapWidth = 800;
                self.mapHeight = 580;
                self.mapTop = 5
                self.mapLeft = 220
                self.tooltipWidth = 100;
                self.tooltipHeight = 80;
                self.markerWidth = 15;
                self.markerHeight = 15;
                self.nodeVisibility = 101;
                self.colAwareness = "#ff9999";
                self.colWillpower = "#bbbbbb";
        self.UI = UI()        
        class Static:
            def __init__(self):
                self.difficulty = { 'level' : 0, 'upgradeChance' : 1.10, 'awarenessResource' : 1.25, 'awarenessUpgrade' : 0.75, 'awarenessGain' : 0.75, 'investigatorChance' : 0.50, 'investigatorKill' : 0.75, 'investigatorWillpower' : 0.75, 'investigatorTurnVisible' : 0, 'investigatorGainWill' : 0.50, 'maxAwareness' : 10}
                self.cults = [{ 'name' : "Cult of Elder God", 'note' : "The cult still lives.", 'longNote' : "At the dawn of humanity the great old ones told their secrets in dreams to the first men, who formed a cult which had never died... Hidden in distant and dark places of the world, waiting for the day when the stars will be right again and the mighty Elder God will rise from his slumber under the deep waters to bring the earth beneath his sway once more.", 'summonStart' : "", 'summonFinish' : "", 'summonFail' : ""},{ 'name' : "Pharaonic Slumber", 'note' : "A group that wants to put the entire world to sleep, feeding on the nightmares of the dreamers.", 'longNote' : "Abhumans from a dark dimension close to ours came to Earth thousands of years ago, trading their magics and technology with the Egyptians for control of their people's minds when they slept, for they fed upon nightmares. With the secret help of the Roman Empire, the Egyptians drove the abhumans into hiding. But they have returned, and their goal has grown with time: the permanent slumber of the world.", 'summonStart' : "As the Pharaonic Slumber's power grows, the world enters a state of controlled drowsiness. People go to bed earlier and sleep later, their dreams plagued with thoughts of sweeping sands and dark figures. Short naps at work become almost commonplace, and as the abhumans feed upon the dreaming energies of the world, everyone feels less and less energetic. All the more reason to take a bit of a rest...", 'summonFinish' : "The world drifts off to sleep, some even slumping to the sidewalk where they were just walking or barely managing to bring their vehicles to a stop. The abhumans come out in force, walking amongst the dreaming populace, feeding hungrily upon the horrid dreams foisted upon them by the dark magics. A few humans manage to keep themselves awake a bit longer on massive doses of amphetamines, but soon they too crash into the darkness of eternal slumber, screaming into unconsciousness as they see the burning red eyes of those who've come to consume their thoughts.", 'summonFail' : "People shake off the dozing state that had captured them. Sales of coffee and cola rocket temporarily, an odd spike that intrigues many commentators, and for a moment humanity is more awake than it has ever been before. Soon, however, old habits return, and some are still plagued by dreams of windswept deserts they have never before seen and cloaked figures that move in a way that somehow feels inhuman, dreams that feel more real than reality."},{ 'name' : "Blooded Mask", 'note' : "A group devoted to ripping away the masks people wear and revealing the insane reality beyond.", 'longNote' : "Those who peer too long into the Abyss find that it stares back at them, and the Blooded Mask has long gazed into the ineffable world beyond our own. Affiliated with no Elder God, or perhaps all of them, the Blooded Mask longs to show humanity the brutal truths that hide behind the consensual reality. The truths drive those who see them insane, filling them with a desire to show others as well, and the Blooded Mask are the original converts.", 'summonStart' : "A rash of cases of schizophrenia and paranoid delusions becomes an epidemic.  World health organizations struggle to understand environmental factors behind the increasing numbers of psychotic breaks and irrational behaviour across the world, unaware of the rituals the Blooded Mask are enacting.  The only clue is an increased incidence of individuals trying to claw their eyes out, often babbling about seeing <i>the truth</i> better without them.", 'summonFinish' : "Even the most stable individuals become gripped by the desire to see beyond the veil. Plucking their eyes out, almost as one, humanity peers out of bloody sockets into the screaming void of alien truth that had, until then, been hidden to most. The Bloody Veil's incantations brought to their climax, the world becomes a madhouse of screaming blind horror, people stumbling through living nightmares in colours their minds were never meant to comprehend, groping past those others wandering in the same strange geometries.", 'summonFail' : "The outbreak of madness draws to a close, the circumstances at its end as mysterious as when it began. Sanity returns to some of those who saw the underlying truth, but those who blinded themselves are relegated to sanitariums around the world, their screaming reverberating in the halls of the buildings, unable to stop seeing the horrifying ur-reality. A small number of painters attempt to incorporate the colours they saw in their madness into their work, and the epileptic seizures their paintings evoke cause the black ops divisions of the world's governments to destroy all evidence of their work."},{ 'name' : "Universal Lambda", 'note' : "Programmers who want to turn humanity into a vast processing machine.", 'longNote' : "In the early seventies, a secret goverment project uncovered the changes necessary to turn any human brain into an efficient, soulless computer.  Little did the project know that it had been subverted by the dark cult. The Universal Lambda works to refine that now-defunct project's results: the turning of every human being into cogs of a huge machine, a distributed network for the vast living intellect of the Elder God.", 'summonStart' : "The Universal Lambda's cybermantic machinations begin to influence the entire world.  People start to walk in unconscious lockstep down the streets; crime and accident rates drop as the rituals rewire minds to be more and more regimented.  People make fewer choices, locking themselves into patterns without realizing the steady loss of free will.", 'summonFinish' : "Their rituals complete, the Universal Lambda turns the world into a well-oiled machine. Bodies still move around, taking part in the same rote behavior they did before, but the minds of the populace are gone. Instead of thinking independent thoughts, humanity's brains run the code that allows The Machine to run in our dimension. The tiny flickers of free will brought upon by every birth are quickly consumed by the overwhelming cybermantic magics enveloping the world; all are just parts of the giant soulless entity... ", 'summonFail' : "The eerily constant behavior of humanity slowly returns to its regular froth of chaos. People still occasionally slip into the robotic state they exhibited mere days before, but the rising rate of accidents and deaths heralds the return of free will, for better or worse."},{ 'name' : "Drowned", 'note' : "Vengeful spirits determined to drown the rest of the world.", 'longNote' : "Over the millennia, hundreds of thousands of people have drowned in the oceans, lakes, and rivers of the world. Most of them pass peacefully into oblivion, but some linger, warped by the experience of their death. Over time, those who remain have gathered together in an undead cabal. They want the rest of the world to join them in their watery graves, and will stop at nothing to make it happen.", 'summonStart' : "It begins to rain. A slow drizzle at first, the entire world is soon enveloped in an unending thunderstorm, water pouring from the heavens without an end in sight. Low-lying regions begin to flood, and it is only a matter of time before even the highest ground is inundated.", 'summonFinish' : "The heavy rains turn torrential.  The sea level rises inexorably as humanity crowds further and further up into the mountains.  Still the rains come, still the waters climb.  Every death in the water adds to the power of the Drowned, the force of the neverending rain.  Many take\n      to boats in an attempt to survive on the surface of the sea, only to find that no ship remains seaworthy; leaks spring up in unlikely places, and soon every vessel succumbs to the inexorable pull of the dark depths below.  The last gasp of humanity is a doomed man standing on the peak of Everest, and then he goes under once. Twice. He is gone.", 'summonFail' : "The rains slacken, first to a light patter, then a drizzle, then nothing but the usual patterns of storms and showers. Commentators argue that the excess water had to come from somewhere, but within days everything seems to have returned to an equilibrium, the ghost rains drying up into nothing.  Scientists are at a loss to explain the phenomenon, but the\n      rest of the world returns to its routine, although many glance at the sky whenever a cloud darkens the day, worried that it might once again begin to rain forever."}];
                self.rituals = [{ 'id' : "summoning", 'name' : "Final Ritual", 'points' : 9, 'note' : "Upon completion this cult will reign over the world unchallenged."}];
        self.Static = Static()

        class Status:
            def __init__(self, Game):
                self.tipPowers = [Game.powerNames[0] + " is needed to gain new followers.",Game.powerNames[1] + " is needed to gain new followers.",Game.powerNames[2] + " is needed to gain new followers.",((Game.powerNames[3] + " are gathered by your neophytes.<br>") + "They are needed for rituals to upgrade your<br>followers ") + "and also for the final ritual of summoning."];
                self.tipConvert = "Cost to convert to ";
                self.tipUpgrade = [("To gain an adept you need " + str(Game.upgradeCost)) + " neophytes and 1 virgin.",("To gain a priest you need " + str(Game.upgradeCost)) + " adepts and 2 virgins.",((((((("To perform the " + Static().rituals[0]['name']) + " you need ") + str(Game.upgradeCost)) + " priests and ") + str(Game.numSummonVirgins)) + " virgins.<br>") + "<li>The more society is aware of the cult the harder it is to ") + "summon Elder God."];
                self.tipFollowers = ["Neophytes can find some virgins if they're lucky.","Adepts can lower society awareness and investigator's willpower.",("3 priests and " + str(Game.numSummonVirgins)) + " virgins are needed to summon the Elder God."];
                self.tipTurns = "Shows the number of turns passed from the start.";
                self.tipAwareness = ((("Shows how much human society is aware of the cult.<br>" + "<li>The greater awareness is the harder it is to do anything:<br>") + "gain new followers, resources or make rituals.<br> ") + "<li>Adepts can lower the society awareness using resources.<br>") + "<li>The more adepts you have the more you can lower awareness each turn.";
                self.tipLowerAwareness = "Your adepts can use resources to lower society awareness.";
                self.tipLowerWillpower = "Your adepts can use resources to lower willpower of an investigator.<br>Cost: ";
                self.tip = "Click to end current turn.";
                self.tipInfo = "Click to view cults information.";
                self.tipMainMenu = "Click to open main menu.";
                self.tipLog = "Click to view message log.";
                self.tipAbout = "Click to go to About page.";
        self.Status = Status(self.Game)

        class Nodes:
            def __init__(self):
                self.names = ["Government official","Corporate worker","University professor","Army officer","Scientist"];
        self.Nodes = Nodes()


# GAME
class Game:
    def __init__(self):
        self.isFinished = True
        self.ui = UI(self)
        self.ui.init()
        self.nodes = None
        self.cults = None
        self.currentPlayer = -1
        self.lastCultID = None;
        self.lastNodeIndex = None;
        self.lines = None;

    def endTurn(self):
        #unset UI highlight status
        for n in self.nodes:
            if n.isHighlighted:
                n.setHighlighted(False)

        # end game?
        if self.isFinished:
            return

        self.currentPlayer = self.currentPlayer + 1
        if self.currentPlayer == len(self.cults):
            self.currentPlayer = 0
            self.turns = self.turns + 1

#        print "Next: " + str(self.getCurrentCult().name) + " turn " + str(self.turns) + "\n"

        self.getCurrentCult().turn()
        self.ui.updateStatus()
        pygame.display.flip()

    def restart(self, players):
        self.currentPlayer = -1
        self.isFinished = False;
        self.ui.clearMap();
        self.ui.clearLog();
        self.ui.log("Game started.", False);
        self.lines = List()
        self.nodes = []
        self.cults = []
        self.lastCultID = 0;
        self.numCults = len(players)

        for i in range(len(players)):
            p = players[i](self, self.ui, i, i)
            self.cults.append(p)

        self.turns = 0;
        self.lastNodeIndex = 0;

        for g in range(options.Game.nodesCount):
            self.spawnNode()

        # create generators
        for i in range(int(0.15 * options.Game.nodesCount)):
            nodeIndex = random.randint(0, options.Game.nodesCount - 1)
            node = self.nodes[nodeIndex]
            powerIndex = 0#

            for ii in range(options.Game.numPowers):
                if node.power[ii] > 0:
                    node.power[ii] = node.power[ii] + 1
                    powerIndex = ii
            while True:
                ii = random.randint(0, options.Game.numPowers - 1)
                if ii != powerIndex:
                    break
            node.powerGenerated[ii] = 1
            node.setGenerator(True)

        # link nodes
        for n in self.nodes:
            minDist = 10000
            nearestNode = None
            for n2 in self.nodes:
                if n != n2:
                    if n.id < n2.id:
                        dist = n.distance(n2)
                        if dist < minDist:
                            nearestNode = n2
                            minDist = dist
                    if n.distance(n2) < options.UI.nodeVisibility:
                        n.links.remove(n2)
                        n2.links.remove(n)
                        n.links.add(n2)
                        n2.links.add(n)
            if n.links.isEmpty():
                print "EMPTY "+str(n.id)
                n.links.remove(nearestNode)
                nearestNode.links.remove(n)
                n.links.add(nearestNode)
                nearestNode.links.add(n)

        # set cults origin
        for p in self.cults:
            p.setOrigin()

        self.ui.updateStatus();

    def spawnNode(self):
        x = 0
        y = 0
        cnt = 0

        while(True):
            x = random.randint(20, ((options.UI.mapWidth - options.UI.markerWidth) - 20))
            y = random.randint(20, ((options.UI.mapHeight - options.UI.markerHeight) - 20))
            cnt = cnt + 1
            if cnt > 100:
                return
            ok = 1
            for n in self.nodes:
                if (x - 30 < n.x and (x + options.UI.markerWidth) + 30 > n.x) and (y - 30 < n.y and (y + options.UI.markerHeight) + 30 > n.y):
                    ok = 0
                    break
            if ok == 1:
                break

        node = Node(self, self.ui, x, y, self.lastNodeIndex)
        self.lastNodeIndex = self.lastNodeIndex + 1

        index = random.randint(0, options.Game.numPowers - 1)
        node.power[index] = 1
        node.displayPower = index
        if options.Game.mapVisible:
            node.setVisible(self.getCurrentCult(), True);
        node.update();
        self.nodes.append(node);

    def getCurrentCult(self):
        return self.cults[self.currentPlayer]


# UI

class UI:
    def __init__(self, game):
        self.game = game
        self.alertWindow = None
        self.debug = None;
        self.info = None;
        self.screen = pygame.display.set_mode((options.UI.winWidth, options.UI.winHeight))


    def powerName(self, i):
        return options.Game.powerNames[i]

    def cultName(self, i, info):
        return info['name']

    def alert(self, s, shadow=False):
        print "Alert: " + s     # TODO

    def chearLog(self):
        print "\n\n\n\n"

    def clearMap(self):
        self.map.clear()

    def finish(self, cult, state):
        msg = "Game over\n"

        if state == "summon":
            msg = msg + cult.getFullName() + " has completed the " + options.Static.rituals[0]['name'] + "\n"

        if state == "conquer":
            msg = msg + cult.getFullName() + " has taken over the world.\n"

        for n in self.game.nodes:
            n.setVisible(self.game.getCurrentCult(), True)

        self.alert(msg,True);

    def init(self):
        self.logWindow = Log(self,self.game);
        self.alertWindow = Alert(self,self.game);
        self.info = Info(self,self.game);
        self.debug = Debug(self,self.game);
        self.status = Status(self,self.game);
        self.map = Map(self,self.game);
        #self.mainMenu = new MainMenu(self,self.game);

    def log(self, s, show=True):
        self.logWindow.add(s,show);

    def msg(self, s):
        print s

    def track(self, action, label, value):
        action = (("cult " + action) + " ") + options.Game.version;
        if(label is None):
            label = "";
        if(value is None):
            value = 0;
        print ("Evil Cult",action,label,value);

    def updateStatus(self):
        self.status.update();

    def clearLog(self):
        self.logWindow.clear()

# LOG

class Log:
    def __init__(self, ui, game):
        self.ui = ui
        self.game = game

    def clear(self):
        print "\n\n\n"

    def add(self, s, show=True):
        print s # TODO use show

# ALERT

class Alert:
    def __init__(self, ui, game):
        self.ui = ui
        self.game = game

# INFO

class Info:
    def __init__(self, ui, game):
        self.ui = ui
        self.game = game

    def show(self):
        s = ""
        i = 0

        for cult in self.game.cults:
            s = s + " - " + self.ui.cultName(i, cult.info)
            if cult.isDead:
                s = s + " (forgotten)"
            else:
                w = ""
                for i1 in range(len(cult.wars)):
                    if cult.wars[i1]:
                        w = w + self.ui.cultName(i1, self.game.cults[i1].info) + " "
                if w != "":
                    s = s + " wars: " + w

            s = s + "\n"

            if cult.hasInvestigator:
                s = s + "Investigator: Level " + (cult.investigator.level + 1) + ", Willpower " + (p.investigator.will)

        print s

#Info.prototype.show = function() {
#        {
#                var _g = 0, _g1 = self.game.cults;
#                while(_g < _g1.length) {
#                        var p = _g1[_g];
#                        ++_g;
#                        s += "<br>";
#                        if(p.hasInvestigator) {
#                        }
#                        if(p.isRitual == True) {
#                                var turns = Std["int"](p.ritualPoints / p.getPriests());
#                                if(p.ritualPoints % p.getPriests() > 0) turns += 1;
#                                s += ((((((((((("Casting <span title='" + p.ritual.note) + "' id='info.ritual") + i) + "' style='color:#ffaaaa'>") + p.ritual.name) + "</span>, ") + (p.ritual.points - p.ritualPoints)) + "/") + p.ritual.points) + " points, ") + turns) + " turns left<br>";
#                        }
#                        if(!p.isDead) {
#                                s += ((((((p.nodes.length + " followers (") + p.getNeophytes()) + " neophytes, ") + p.getAdepts()) + " adepts, ") + p.getPriests()) + " priests)";
#                                if(p.isParalyzed) s += " Paralyzed";
#                                s += "<br>";
#                        }
#                        s += ("<span id='info.toggleNote" + i) + "' style='height:10; width:10; font-size:12px; border: 1px solid #777'>+</span>";
#                        s += "<br>";
#                        s += ((("<span id='info.note" + i) + "'>") + p.info.note) + "</span>";
#                        s += ((("<span id='info.longnote" + i) + "'>") + p.info.longNote) + "</span>";
#                        s += "</div><hr>";
#                        i++;
#                }
#        }
#        self.window.style.visibility = "visible";
#}


# DEBUG

class Debug:
    def __init__(self, ui, game):
        self.ui = ui
        self.game = game



# STATUS

class Status:
    def __init__(self, ui, game):
        self.ui = ui
        self.game = game

#Status.prototype.onConvert = function(event) {
#        if(self.game.isFinished) return;
#        var from = Std.parseInt(Tools.getTarget(event).id.substr(14,1));
#        var to = Std.parseInt(Tools.getTarget(event).id.substr(15,1));
#        self.game.player.convert(from,to);
#}
#Status.prototype.onDebug = function(event) {
#        if(self.game.isFinished) return;
#        self.ui.debug.show();
#}
#Status.prototype.onEndTurn = function(event) {
#        if(self.game.isFinished) return;
#        self.game.endTurn();
#}
#Status.prototype.onInfo = function(event) {
#        self.ui.info.show();
#}
#Status.prototype.onLog = function(event) {
#        self.ui.logWindow.show();
#}
#Status.prototype.onLowerAwareness = function(event) {
#        if(self.game.isFinished) return;
#        var power = Std.parseInt(Tools.getTarget(event).id.substr(21,1));
#        self.game.player.lowerAwareness(power);
#}
#Status.prototype.onLowerWillpower = function(event) {
#        if(self.game.isFinished) return;
#        var power = Std.parseInt(Tools.getTarget(event).id.substr(21,1));
#        self.game.player.lowerWillpower(power);
#}
#Status.prototype.onMainMenu = function(event) {
#        self.ui.mainMenu.show();
#}
#Status.prototype.onUpgrade = function(event) {
#        if(self.game.isFinished) return;
#        var lvl = Std.parseInt(Tools.getTarget(event).id.substr(14,1));
#        self.game.player.upgrade(lvl);
#}

    def update(self):
        pygame.draw.rect(self.ui.screen, (0, 0, 0), Rect(0, 0, self.ui.map.left, options.UI.winHeight))

        cult = self.game.getCurrentCult();
        s = "- CULT"
        s += "\n" + cult.name
        s += "\n- FOLLOWERS"
        for i in range(len(options.Game.followerNames)):
            s += "\n" + options.Game.followerNames[i] + " " + str(cult.getNumFollowers(i))

        s += "\n- RESOURCES"
        for i in range(len(options.Game.powerNames)):
            s += "\n" + options.Game.powerNames[i] + " " + str(int(cult.power[i])) + " +0-"
            if i < options.Game.numPowers:
                s += str(int(cult.powerMod[i]))
            else:
                s += str(int(cult.getNeophytes() / 4.0 - 0.5))
        s += "\n- STATS"
        s += "\nAwareness " + str(cult.awareness)
        s += "\nTurn " + str(self.game.turns)
        if cult.hasInvestigator:
            s += "\nInvestigator (" + str(cult.investigator.will) + "@" +str(cult.investigator.level + 1) + ")"
            if cult.investigator.isHidden:
                s += "(hidden)"
            
        y = 5
        split = s.split("\n")
        for line in split:
            s = font.render(line, True, (255, 255, 255))
            self.ui.screen.blit(s, (5, y))
            y += s.get_height()

#Status.prototype.update = function() {
#        {
#                var _g1 = 0, _g = (Game.numPowers + 1);
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        var s = ((Status.tipPowers[i] + "<br>Chance to gain each unit: <span style='color:white'>") + self.game.player.getResourceChance()) + "%</span>";
#                        self.updateTip("status.powerMark" + i,s);
#                        self.updateTip("status.powerName" + i,s);
#                }
#        }
#        {
#                var _g1 = 0, _g = Game.followerLevels;
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        self.updateTip("status.follower" + i,Status.tipFollowers[i]);
#                        self.updateTip("status.upgrade" + i,((Status.tipUpgrade[i] + "<br>Chance of success: <span style='color:white'>") + self.game.player.getUpgradeChance(i)) + "%</span>");
#                }
#        }
#        self.updateTip("status.followers1",(self.game.player.adeptsUsed + " used of ") + self.game.player.getAdepts());
#        {
#                var _g1 = 0, _g = (Game.numPowers + 1);
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        var _g3 = 0, _g2 = Game.numPowers;
#                        while(_g3 < _g2) {
#                                var ii = _g3++;
#                                if(i == ii) continue;
#                                var c = js.Lib.document.getElementById(("status.convert" + i) + ii);
#                                c.style.visibility = ((self.game.player.power[i] >= Game.powerConversionCost[i]?"visible":"hidden"));
#                        }
#                }
#        }
#        {
#                var _g1 = 0, _g = Game.followerLevels;
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        var s = "" + self.game.player.getNumFollowers(i);
#                        if(i == 1 and self.game.player.getAdepts() > 0) {
#                                s = ("<span style='color:#55dd55'>" + (self.game.player.getAdepts() - self.game.player.adeptsUsed)) + "</span>";
#                        }
#                        js.Lib.document.getElementById("status.followers" + i).innerHTML = s;
#                }
#        }
#        {
#                var _g1 = 0, _g = (Game.numPowers + 1);
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        js.Lib.document.getElementById("status.power" + i).innerHTML = ("<b>" + self.game.player.power[i]) + "</b>";
#                        if(i == 3) js.Lib.document.getElementById("status.powerMod3").innerHTML = " +0-" + Std["int"](self.game.player.getNeophytes() / 4 - 0.5);
#                        else js.Lib.document.getElementById("status.powerMod" + i).innerHTML = " +0-" + self.game.player.powerMod[i];
#                }
#        }
#        js.Lib.document.getElementById("status.turns").innerHTML = "" + self.game.turns;
#        js.Lib.document.getElementById("status.awareness").innerHTML = ("" + self.game.player.awareness) + "%";
#        {
#                var _g1 = 0, _g = Game.numPowers;
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        js.Lib.document.getElementById("status.lowerAwareness" + i).style.visibility = "hidden";
#                }
#        }
#        if(self.game.player.adeptsUsed < self.game.player.getAdepts() and self.game.player.getAdepts() > 0 and self.game.player.awareness > 0) {
#                var _g1 = 0, _g = Game.numPowers;
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        if(self.game.player.power[i] > 0) js.Lib.document.getElementById("status.lowerAwareness" + i).style.visibility = "visible";
#                }
#        }
#        {
#                var _g1 = 0, _g = Game.numPowers;
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        js.Lib.document.getElementById("status.lowerWillpower" + i).style.visibility = "hidden";
#                }
#        }
#        if(self.game.player.hasInvestigator and !self.game.player.investigator.isHidden and self.game.player.adeptsUsed < self.game.player.getAdepts() and self.game.player.getAdepts() > 0) {
#                var _g1 = 0, _g = Game.numPowers;
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        if(self.game.player.power[i] >= Game.willPowerCost) js.Lib.document.getElementById("status.lowerWillpower" + i).style.visibility = "visible";
#                }
#        }
#        {
#                var _g1 = 0, _g = (Game.followerNames.length - 1);
#                while(_g1 < _g) {
#                        var i = _g1++;
#                        js.Lib.document.getElementById("status.upgrade" + i).style.visibility = (((self.game.player.getNumFollowers(i) >= Game.upgradeCost and self.game.player.getVirgins() >= i + 1)?"visible":"hidden"));
#                }
#        }
#        js.Lib.document.getElementById("status.upgrade2").style.visibility = (((self.game.player.getPriests() >= Game.upgradeCost and self.game.player.getVirgins() >= Game.numSummonVirgins and !self.game.player.isRitual)?"visible":"hidden"));
#}
#Status.prototype.updateTip = function(name,tip) {
#        name = "#" + name;
#        if(name.indexOf(".") > 0) {
#                name = (name.substr(0,name.indexOf(".")) + "\\") + name.substr(name.indexOf("."));
#        }
#        new JQuery(name).attr("tooltipText",tip);
#}


# MAP

class Map:
    def __init__(self, ui, game):
        self.ui = ui
        self.game = game
        self.top = options.UI.mapTop
        self.left = options.UI.mapLeft
        self.width = options.UI.mapWidth
        self.height = options.UI.mapHeight

    def clear(self):
#        print "CLEAR"
        pygame.draw.rect(self.ui.screen, (0, 0, 0), Rect(self.left, self.top, self.width, self.height))
        pygame.draw.rect(self.ui.screen, (40, 40, 40), Rect(self.left, self.top, self.width, self.height), 1)

    def redraw(self):
        self.clear()
        for node in self.game.nodes:
            node.update()
#Map.prototype.onNodeClick = function(event) {
#       if(self.game.isFinished) return;
#       self.game.player.activate(Tools.getTarget(event).node);
#}

# LIST

class List:
    def __init__(self):
        self.q = []

    def add(self, item):
        for i in range(len(self.q)):
            if item == self.q[i]:
                return
        self.q.append(item)

    def clear(self):
        self.q = []

    def filter(self, fn):
        l2 = List()
        for item in self.q:
            if fn(item):
                l2.add(item)
        return l2

    def first(self):
        if len(self.q) > 0:
            return self.q[0]
        return None
    

    def isEmpty(self):
        return len(self.q) == 0

#List.prototype.iterator = function() {
#        return { h : self.h, hasNext : function() {
#                return (self.h != None);
#        }, next : function() {
#                if(self.h == None) return None;
#                var x = self.h[0];
#                self.h = self.h[1];
#                return x;
#        }}
#}

#List.prototype.join = function(sep) {
#        var s = new StringBuf();
#        var first = True;
#        var l = self.h;
#        while(l != None) {
#                if(first) first = False;
#                else s.b[s.b.length] = sep;
#                s.b[s.b.length] = l[0];
#                l = l[1];
#        }
#        return s.b.join("");
#}

    def last(self):
        if len(self.q) > 0:
            return self.q[len(self.q)-1]
        return None

    def map(self, fn):
        res = List()
        for item in self.q:
            res.add(fn(item))
        return res

    def pop(self):
        if len(self.q) == 0:
            return None
        return self.q.pop()

    def push(self, item):
        self.q.append(item)

    def remove(self, item):
        for i in range(len(self.q)):
            if item == self.q[i]:
                self.q.pop(i)
                return True
        return False

#List.prototype.toString = function() {
#        var s = new StringBuf();
#        var first = True;
#        var l = self.h;
#        s.b[s.b.length] = "{";
#        while(l != None) {
#                if(first) first = False;
#                else s.b[s.b.length] = ", ";
#                s.b[s.b.length] = Std.string(l[0]);
#                l = l[1];
#        }
#        s.b[s.b.length] = "}";
#        return s.b.join("");
#}


# CULT

class Cult:
    def __init__(self, game, ui, id, infoId):
        self.game = game
        self.ui = ui
        self.id = id
        self.infoId = infoId
        self.power = [0, 0, 0, 0]
        self.powerMod = [0, 0, 0, 0]
        self.wars = [False, False, False, False]
        self.adeptsUsed = 0
        self.setAwareness(0)
        self.nodes = List()
        self.origin = None
        self.neophytes = 0
        self.investigatorTimeout = 0
        self.hasInvestigator = False
        self.isRitual = False
        self.isParalyzed = False
        self.isDead = False
        self.isDebugInvisible = False
        self.info = options.Static.cults[infoId]
        self.name = self.info['name']
        
    # take over a node
    def activate(self, node):
#        print "Activate" + self.getFullName() + " node " + str(node.id)
        # do we have an origin?
        if(self.isParalyzed):
            self.ui.alert("Cult is paralyzed without the Origin.");
            return "";

        # node already owned by me?
        if(node.owner == self):
            return "isOwner";

        # protected generator?
        if(node.isGenerator and node.owner is not None):
            cnt = 0
            for n in node.links.q:
                if n.owner == node.owner:
                    cnt = cnt + 1
            if cnt >= 3:
                return "hasLinks";

        # can conquer it?
        for i in range(options.Game.numPowers):
            if self.power[i] < node.power[i]:
                return "notEnoughPower";

        # pay the cost
        for i in range(options.Game.numPowers):
            self.power[i] = self.power[i] - node.power[i]

        # may randomly fail to acquire
        if random.randint(0, 100) > self.getGainChance(node):
            return "failure"            

        # node conquered!
        if node.level > 0:
            node.level = node.level - 1
        node.setOwner(self);
        self.checkVictory();
        node.setHighlighted(True);
        return "ok";

    # do I have enough resources for the node?
    def canActivate(self,node):
        for i in range(options.Game.numPowers):
            if self.power[i] < node.power[i]:
                return False
        return True

    def checkDeath(self):
        if not self.nodes.isEmpty() or self.isDead:
            return;
        self.ui.log(self.getFullName() + " has been destroyed, forgotten by time.");
        self.isDead = True;

        # not at war anymore
        for cult in self.game.cults:
            cult.wars[self.id] = False

        for i in range(len(self.wars)):
            self.wars[i] = False

        self.hasInvestigator = False;
        self.investigator = None;
        self.checkVictory();

    def checkVictory(self):
        alive = 0
        for cult in self.game.cults:
            if not cult.isDead:
                alive = alive + 1
        if alive == 1:
            self.game.isFinished = True;
            self.ui.finish(self,"conquer");
            
    def convert(self,from_,to):
        if(self.power[from_] < options.Game.powerConversionCost[from_]):
#            self.ui.log("Not enough " + options.Game.powerNames[from_])
            return;
        self.power[from_] -= options.Game.powerConversionCost[from_];
        self.power[to] += 1;
        self.ui.updateStatus();

    def declareWar(self,cult):
        if(cult.wars[self.id]):
            return;
        cult.wars[self.id] = True;
        self.wars[cult.id] = True;
#        self.ui.log(((self.getFullName() + " has declared war against ") + cult.getFullName()) + ".");

    def getAdepts(self):
        return self.getNumFollowers(1);

    def getFullName(self):
        return self.name#self.ui.cultName(self.id,self.info);

    def getGainChance(self, node):
        ch = 0;
        if(not node.isGenerator):
            ch = 99 - int(self.awareness * options.Static.difficulty['awarenessGain']);
        else:
            ch = 99 - int((self.awareness * 2) * options.Static.difficulty['awarenessGain']);
        if(ch < 1):
            ch = 1;
        return ch;

    def getInvestigatorChance(self):
        return int(((20 * self.getPriests() + 5 * self.getAdepts()) + 0.5 * self.getNeophytes()) * options.Static.difficulty['investigatorChance']);

    def getNeophytes(self):
        return self.getNumFollowers(0);

    def getNumFollowers(self, level):
        cnt = 0;
        for node in self.nodes.q:
            if node.level == level:
                cnt += 1

        return cnt;

    def getPriests(self):
        return self.getNumFollowers(2);

    def getResourceChance(self):
        ch = 99 - int(options.Static.difficulty['awarenessResource'] * self.awareness);
        if(ch < 1):
            ch = 1;
        return ch;

    def getUpgradeChance(self,level):
        ch = 0;
        if(level == 0):
            ch = int(99 * options.Static.difficulty['upgradeChance'] - self.awareness * options.Static.difficulty['awarenessUpgrade']);
        elif(level == 1):
            ch = int(80 * options.Static.difficulty['upgradeChance'] - (self.awareness * 1.5) * options.Static.difficulty['awarenessUpgrade']);
        elif(level == 2):
            ch = int(75 * options.Static.difficulty['upgradeChance'] - (self.awareness * 2) * options.Static.difficulty['awarenessUpgrade']);
        if(ch < 1):
            ch = 1;
        if(ch > 99):
            ch = 99;
        return ch;

    def getVirgins(self):
        return self.power[3];

    def loseNode(self, node,cult):
        self.setAwareness(self.awareness + 1)
        
        self.ui.updateStatus();
        if(cult is not None and not self.nodes.isEmpty()):
            cult.declareWar(self);
        if(self.origin == node):
            self.loseOrigin();
        node.update();
        self.checkDeath();

    def loseOrigin(self):
        self.ui.log(self.getFullName() + " has lost its Origin.");
        if(self.isRitual):
                self.isRitual = False;
                self.ui.log(("The execution of " + self.ritual['name']) + " has been stopped.");

        ok = False;
        self.origin = None;
        for node in self.nodes.q:
            if node.level == 2:
                self.origin = node
                ok = True
                break

        if not ok:
                self.ui.log(("With no priests left " + self.getFullName()) + " is completely paralyzed.");
                self.isParalyzed = True;
        else:
                self.ui.log(("Another priest becomes the Origin of " + self.getFullName()) + ".");
                self.origin.update();

    def lowerAwareness(self,pwr):
        if(self.awareness == 0 or self.adeptsUsed >= self.getAdepts() or pwr == 3):
           return;
        self.setAwareness(self.awareness - 2)
        if(self.awareness < 0):
            self.setAwareness(0);
        self.power[pwr] -= 1;
        self.adeptsUsed += 1;
        self.ui.updateStatus();

    def lowerWillpower(self, pwr):
        if(not self.hasInvestigator or self.adeptsUsed >= self.getAdepts() or pwr == 3 or self.power[pwr] < options.Game.willPowerCost or self.investigator.isHidden):
           return;
        self.power[pwr] -= options.Game.willPowerCost;
        if(random.randint(0, 100) < 30 * options.Static.difficulty['investigatorWillpower']):
            self.ui.alert("You have failed to shatter the will of the investigator.");
            self.ui.updateStatus();
            return;

        self.investigator.will -= 1;
        if(self.investigator.will <= 0):
                self.investigator = None;
                self.hasInvestigator = False;
                self.ui.log(("The investigator of the " + self.getFullName()) + " has disappeared.");
                self.investigatorTimeout = 3;
        self.adeptsUsed += 1;
        self.ui.updateStatus();

    def makePeace(self, cult):
        if(not cult.wars[self.id]):
           return;
        cult.wars[self.id] = False;
        self.wars[cult.id] = False;
        self.ui.log(((self.getFullName() + " has made peace with ") + cult.getFullName()) + ".");

    def removeCloseGenerators(self):
        for node in self.origin.links.q:
            if node.owner is None and node.isGenerator:
                node.setGenerator(False)

    def ritualFinish(self):
        if(self.ritual['id'] == "summoning"):
            self.summonFinish();
        self.isRitual = False;

    def setAwareness(self, v):
        self.awareness = v;
        for node in self.game.nodes:
            if node.visibility[self.id] and node.owner != self:
                node.update()
        return v;

    def setOrigin(self):
        index = -1
        while True:
            index = random.randint(0, len(self.game.nodes)-1)
            node = self.game.nodes[index]
#            print "Origin " + self.getFullName() + " node " + str(index)
            if node.owner is not None:
                continue
            ok = 1
            for p in self.game.cults:
                if p.origin is not None and node.distance(p.origin) < options.UI.nodeVisibility + 50:
                    ok = 0
                    break
            if ok == 1:
                break
            
        self.origin = self.game.nodes[index];
        self.origin.owner = self;
        self.nodes.add(self.origin);
        self.origin.update();

        for i in range(options.Game.numPowers):
            if self.origin.power[i] > 0:
                self.origin.powerGenerated[i] = 1
                self.powerMod[i] += 1

        self.neophytes += 1;
        self.origin.setGenerator(True);
        self.origin.setVisible(self,True);
        self.origin.showLinks();

        # ??????
        for i in range(options.Game.numPowers):
            # ???????
            #self.power[i] += round(self.origin.powerGenerated[i]);
            if(random.random() < 0.5):
                self.origin.power[i] += 1;

        self.origin.update();
#        self.removeCloseGenerators();

    def setVirgins(self,v):
        self.power[3] = v;
        return v;

    def summonFinish(self):
        if(random.randint(0,100) > self.getUpgradeChance(2)):
            for node in self.nodes.q:
                if node.level == 2:
                    node.level = 0;
                    node.update();
                    break;
            self.ui.alert((((self.getFullName() + " has failed to perform the ") + options.Static.rituals[0]['name']) + ".<br><br>") + self.info['summonFail']);
            self.ui.log(((self.getFullName() + " has failed the ") + options.Static.rituals[0]['name']) + ".");
            return;
        self.game.isFinished = True;
        self.ui.finish(self,"summon");
        self.ui.log("Game over.");

    def summonStart(self):
        if(self.isRitual):
            self.ui.alert("You must first finish the current ritual before starting another.");
            return;
        self.setVirgins(self.getVirgins() - options.Game.numSummonVirgins)
        self.isRitual = True;
        self.ritual = options.Static.rituals[0];
        self.ritualPoints = self.ritual['points'];

        for cult in self.game.cults:
            if cult != self and not self.isDead:
                cult.wars[self.id] = True
                self.wars[cult.id] = True

        self.ui.alert((((self.getFullName() + " has started the ") + self.ritual['name']) + ".<br><br>") + options.Static.cults[self.id]['summonStart']);
        self.ui.log(((self.getFullName() + " has started the ") + self.ritual['name']) + ".");
        self.ui.updateStatus();


    def turn(self):
        if((self.getPriests() > 0 or self.getAdepts() > 0) and not self.hasInvestigator and random.randint(0,100) < self.getInvestigatorChance() and self.investigatorTimeout == 0):
            self.hasInvestigator = True;
            self.ui.log(("An investigator has found out about " + self.getFullName()) + ".");
            self.investigator = Investigator(self,self.ui);
            self.ui.updateStatus();
        if(self.investigatorTimeout > 0):
            self.investigatorTimeout -= 1
            
        if(self.isRitual):
            self.ritualPoints -= self.getPriests();
            if(self.ritualPoints <= 0):
                self.ritualFinish();
            if(self.game.isFinished):
                self.ui.updateStatus();
                self.ui.map.redraw()
                return;
        
        self.powerMod = [0,0,0,0];

        for node in self.nodes.q:
            if node.isGenerator:
                for i in range(options.Game.numPowers):
                    if random.randint(0, 100) < self.getResourceChance():
                        self.power[i] += int(round(node.powerGenerated[i]));
                    self.powerMod[i] += int(round(node.powerGenerated[i]));

        value = int(round(random.random() * (self.getNeophytes() / 4.0 - 0.5)))
        self.setVirgins(self.getVirgins() + value)
        self.adeptsUsed = 0;
        if(self.hasInvestigator):
            self.investigator.turn();
        self.ui.updateStatus();
        self.ui.map.redraw()
        pygame.display.flip()
#        print str(self.power)
        self.handleTurn()

    def handleTurn(self):
        print "TODO: cult.handleTurn"
        pass
    
    def upgrade(self, level):
        if((level == 2 and self.getVirgins() < options.Game.numSummonVirgins) or (level < 2 and self.getVirgins() < level + 1)):
            self.ui.log("Not enough virgins");
            return;
        
        if(level == 2):
            self.summonStart();
            return;
        self.setVirgins(self.getVirgins() - (level + 1))

        if (random.randint(0, 100) > self.getUpgradeChance(level)):
            self.ui.log("Ritual failed.");
            self.ui.updateStatus();
            return;

        self.setAwareness(self.awareness + level);

        ok = False;
        upNode = None;
        if(self.origin is not None and self.origin.level == level):
            self.origin.upgrade();
            upNode = self.origin;
            ok = True;

        if not ok:
            for node in self.nodes.q:
                if node.level == level and node.isGenerator:
                    node.upgrade()
                    upNode = node
                    ok = True
                    break

        if not ok:
            for node in self.nodes.q:
                if node.level == level:
                    node.upgrade()
                    upNode = node
                    ok = True
                    break

        self.ui.updateStatus();
        if self.getPriests() >= 2:
            self.ui.log(((self.getFullName() + " has ") + str(self.getPriests())) + " priests. Be careful.");
        if(self.isParalyzed and self.getPriests() >= 1):
                self.isParalyzed = False;
                self.origin = upNode;
                self.origin.update();
                self.ui.log(self.getFullName() + " gains a priest and is no longer paralyzed.");


# default AI

class CultAI(Cult):
    def aiActivateNodeByConvert(self, node):
        resNeed = -1
        for i in range(options.Game.numPowers):
            if (self.power[i] < node.power[i]):
                resNeed = i;
        resConv = -1;
        for i in range(options.Game.numPowers):
            if(i != resNeed):
#                print "tryConvert " + str(int(round(self.power[i] / options.Game.powerConversionCost[i])))+ " " + str(node.power[resNeed])
                if(float(self.power[i] / options.Game.powerConversionCost[i]) >= node.power[resNeed]):
                    resConv = i;
#        print "aiActivateNodeByConvert " + str(self.power)+ " " + str(node.power) + " " + options.Game.powerNames[resConv]
        if(resConv < 0):
            return;
        for i in range(options.Game.numPowers):
            self.convert(resConv,resNeed);
        self.activate(node);

    def aiLowerAwareness(self):
        if((self.awareness < options.Static.difficulty['maxAwareness'] and not self.hasInvestigator) or (self.awareness < 5 and self.hasInvestigator) or self.getAdepts() == 0 or self.adeptsUsed >= self.getAdepts()):
            return;
        prevAwareness = self.awareness;
        for i in range(options.Game.numPowers):
            while(self.power[i] > 0 and self.adeptsUsed < self.getAdepts() and self.awareness >= options.Static.difficulty['maxAwareness']):
                self.lowerAwareness(i);
        if(options.Game.debugAI and self.awareness != prevAwareness):
            print(((((self.name + " awareness ") + prevAwareness) + "% -> ") + self.awareness) + "%",{ fileName : "AI.hx", lineNumber : 297, className : "AI", methodName : "aiLowerAwareness"});

    def aiLowerAwarenessHard(self):
        if(self.awareness == 0 or self.getAdepts() == 0):
            return;
        prevAwareness = self.awareness;
        while(self.getVirgins() > 0 and self.adeptsUsed < self.getAdepts() and self.awareness >= 0):
            self.convert(3,0);
            self.lowerAwareness(0);
        if(options.Game.debugAI and self.awareness != prevAwareness):
            print(((((self.name + " virgin awareness ") + prevAwareness) + "% -> ") + self.awareness) + "%",{ fileName : "AI.hx", lineNumber : 276, className : "AI", methodName : "aiLowerAwarenessHard"});

    def aiLowerWillpower(self):
        if(not self.hasInvestigator or self.investigator.isHidden or self.getAdepts() == 0):
            return;
        for i in range(options.Game.numPowers):
            self.lowerWillpower(i);
            self.lowerWillpower(i);

    def aiSummon(self):
        if(self.getPriests() < 3 or self.getVirgins() < 9 or self.getUpgradeChance(2) < 50 or self.isRitual):
            return;
#        if(options.Game.debugAI):
#            print(self.name + " TRY SUMMON!",{ fileName : "AI.hx", lineNumber : 308, className : "AI", methodName : "aiSummon"});
        self.summonStart();

    def aiTryPeace(self):
        if(self.isRitual):
            return;
        ok = False;
        for cult in self.game.cults:
            if cult.isRitual:
                ok = True
                break
        if not ok:
            return
        for i in range(len(self.game.cults)):
            if self.wars[i] and not self.game.cults[i].isRitual:
                if(random.randint(0, 100) > 30):
                    continue;
                self.makePeace(self.game.cults[i]);

    def handleTurn(self):
        if(self.isParalyzed and self.hasInvestigator):
            self.aiLowerAwarenessHard();
            self.aiLowerWillpower();
            return;
        if(self.isParalyzed):
            self.aiLowerWillpower();
            self.aiUpgradeFollowers();
            return;
        if(self.hasInvestigator):
            self.aiLowerWillpower();
            return;
        self.aiUpgradeFollowers();
        if(self.isRitual and self.ritual['id'] == "summoning"):
            self.aiLowerAwarenessHard();
        self.aiLowerAwareness();
        self.aiSummon();
        if(self.awareness > options.Static.difficulty['maxAwareness'] and self.getAdepts() > 0):
            return;
        
        potentialTargets = []
        for node in self.game.nodes:
            if(node.owner == self or not node.visibility[self.id] or (node.owner is not None and node.owner.isDebugInvisible)):
                continue;
            item = { 'node' : node, 'priority' : 0}
            if(node.owner is not None and node.level == 2 and node.owner.isRitual and node.owner.ritual['id'] == "summoning"):
                item['priority'] += 3;
            if(node.owner is None):
                item['priority'] += 1;
            if(node.owner is not None and self.wars[node.owner.id]):
                item['priority'] += 1;
            elif(node.owner is not None and node.owner.isRitual and node.owner.ritual.id == "summoning"):
                item['priority'] += 2
            elif(node.owner is not None):
                item['priority'] += -1;
            if(node.isGenerator and not node.isProtected):
                item['priority'] += 2
            if(self.canActivate(node)):
                item['priority'] += 1
            potentialTargets.append(item);
        potentialTargets.sort(lambda a, b: cmp(b['priority'], a['priority']));
        for item in potentialTargets:
            node = item['node']
            ret = self.activate(node)
#            print "Consider: " + str(item['node'].id) + "@" +str(item['priority']) + " " + str(item['node'].isGenerator) + " " + ret
            if(ret == "ok"):
                continue;
            if(ret == "notEnoughPower"):
                self.aiActivateNodeByConvert(node);
            elif(ret == "hasLinks"):
                pass
        self.aiTryPeace();

    def aiUpgradeFollowers(self):
        if(self.getVirgins() == 0):
            return;
        if(self.getAdepts() < 5 and self.getUpgradeChance(0) > 70 and self.getVirgins() > 0):
            while(True):
                if(self.getVirgins() < 1 or self.getAdepts() >= 5):
                    break;
                self.upgrade(0);
#                if(options.Game.debugAI):
#                    print((self.name + " upgrade neophyte, adepts: ") + self.getAdepts(),{ fileName : "AI.hx", lineNumber : 220, className : "AI", methodName : "aiUpgradeFollowers"});
            return;
        if(self.getPriests() < 3 and self.getUpgradeChance(1) > 60 and self.getVirgins() > 1):
            while(True):
                if(self.getVirgins() < 2 or self.getPriests() >= 3):
                    break;
                self.upgrade(1);
#                if(options.Game.debugAI):
#                    print((("!!! " + self.name) + " upgrade adept, priests: ") + self.getPriests(),{ fileName : "AI.hx", lineNumber : 236, className : "AI", methodName : "aiUpgradeFollowers"});
            return;


# NODE

class Node:
    def __init__(self, game, ui, x, y, idx):
        self.game = game;
        self.ui = ui
        self.id = idx
        self.lines = List();
        self.links = List();
        self.visibility = []
        for i in range(game.numCults):
            self.visibility.append(False)
        self.isGenerator = False;
        self.power = [0,0,0];
        self.powerGenerated = [0,0,0];
        self.level = 0;
        self.owner = None;
        self.name = options.Nodes.names[random.randint(0, len(options.Nodes.names) - 1)];
        self.x = x;
        self.y = y;
        self.centerX = self.x + round(options.UI.markerWidth / 2);
        self.centerY = self.y + round(options.UI.markerHeight / 2);
        self.uiNode = UINode(self.game,self.ui,self);
        self.isHighlighted = False

    def clearLines(self):
        if(self.owner == None):
            return;

        for line in self.lines.q:
            line.clear()
            self.game.lines.remove(line)
            line.startNode.lines.remove(line)
            line.endNode.lines.remove(line)

    def distance(self, node):
        return math.sqrt((node.x - self.x) * (node.x - self.x) + (node.y - self.y) * (node.y - self.y));

    def isVisible(self,c):
        return self.visibility[c.id];

    def paintLines(self):
        hasLine = False;
        for n in self.links.q:
            if(n.owner == self.owner):
                if(n.visibility[self.game.currentPlayer] and self.visibility[self.game.currentPlayer]):
                    Line_paint(self.ui.map,self.owner,n,self);
                else:
                    Line_paint(self.ui.map,None,n,self);
                hasLine = True;

        if(hasLine):
            return;
        dist = 10000;
        nc = None;

# ??? redundant?
#        if self.owner is None:
#            return
#
#        for n in self.owner.nodes.q:
#            if(self != n and self.distance(n) < dist):
#                dist = self.distance(n);
#                nc = n;
#
#        l = Line_paint(self.ui.map,self.owner,nc,self);
#            l.setVisible(True);

    def removeOwner(self):
        if(self.owner is None):
            return;
        prevOwner = self.owner;
        self.clearLines();
        self.owner.nodes.remove(self);
        self.owner = None;
        self.level = 0;
        self.update();
        self.updateLinkVisibility(prevOwner);

        for n in self.links.q:
            n.update()

        for i in range(options.Game.numPowers):
            if self.power[i] > 2:
                self.power[i] = 2

        if(prevOwner is not None):
            prevOwner.loseNode(self, None);

    def setGenerator(self, isgen):
        self.isGenerator = isgen;
        self.update();

    def setHighlighted(self, isHL):
        self.isHighlighted = isHL;
        self.uiNode.setHighlighted();

    def setOwner(self, c):
#        print "SetOwner " + str(self.id) + " to " + c.getFullName()
        prevOwner = self.owner;
        if(self.isGenerator):
            for i in range(options.Game.numPowers):
                c.powerMod[i] += int(round(self.powerGenerated[i]));
                       
        self.clearLines();
        if(self.owner is None):
            for i in range(options.Game.numPowers):
                if(self.power[i] > 0):
                    self.power[i]+=1;

        if(self.owner is not None):
            self.owner.nodes.remove(self);
        self.owner = c;
        self.owner.nodes.add(self);
        self.update();
        self.showLinks();
        if(prevOwner is not None):
            self.updateLinkVisibility(prevOwner);
        if(self.isGenerator):
            self.owner.setAwareness(self.owner.awareness + 2)
        else:
            self.owner.setAwareness(self.owner.awareness + 1)

        self.paintLines();
        self.ui.updateStatus();

        for n in self.links.q:
            n.update()

        if(prevOwner is not None):
            prevOwner.loseNode(self,self.owner);

    def setVisible(self, cult,v):
        self.visibility[cult.id] = v;
        self.uiNode.setVisible(cult,v);
        if(options.Game.mapVisible):
            v = True;
        for l in self.lines.q:
            l.setVisible(v)
        if(self.isHighlighted):
            self.setHighlighted(v);

    def showLinks(self):
        for n in self.links.q:
            n.setVisible(self.owner,True);
                       
    def update(self):
        self.isProtected = False;
        if(self.isGenerator and self.owner is not None):
            cnt = 0;
            for n in self.links.q:
                if n.owner == self.owner:
                    cnt += 1

            if(cnt >= 3):
                self.isProtected = True;
        self.paintLines()
        self.uiNode.update();

    def updateLinkVisibility(self, cult):
        for n in self.links.q:
            if(n.visibility[cult.id] and n.owner != cult):
                vis = False;
                for n2 in n.links.q:
                    if(n2.owner == cult):
                        vis = True;
                n.setVisible(cult,vis);
                n.update();

    def upgrade(self):
        if(self.level >= len(options.Game.followerNames)):
            return;
        for i in range(options.Game.numPowers):
            if self.power[i] > 0:
                self.power[i] += 1
        self.level += 1
        self.update();


def Line_paint(map, player, startNode, endNode):
    ox = options.UI.mapLeft + 0.5 * options.UI.markerWidth
    oy = options.UI.mapTop + 0.5 * options.UI.markerHeight
    if player is not None:
        color = options.Game.lineColors[player.id]
    else:
        color = (0, 0, 0)
    pygame.draw.line(map.ui.screen, color, (startNode.x+ox, startNode.y+oy), (endNode.x+ox, endNode.y+oy))

# UINODE

class UINode:
    def __init__(self, game, ui, n):
        self.game = game;
        self.ui = ui;
        self.node = n;

    def setHighlighted(self):
        self.update();

    def setVisible(self, cult,vis):
        surface = self.ui.screen
        x0 = self.ui.map.left
        y0 = self.ui.map.top
        self.node.visibility[cult.id] = vis
        self.update()

    def update(self):
#        print "TODO uinode update"
        surface = self.ui.screen    # todo map per player
        x0 = self.ui.map.left
        y0 = self.ui.map.top
        visible = self.node.visibility[self.game.currentPlayer]

#        print "Update node " + str(self.node.id) + " owned by " + str(self.node.owner)

        if not visible:
            pygame.draw.rect(self.ui.screen, (0, 0, 0), Rect(x0+self.node.x, y0+self.node.y, options.UI.markerWidth, options.UI.markerHeight))
            return
        
#        var s = "";
#        if(Game.debugNear) {
#                s += ("Node " + self.node.id) + "<br>";
#                { var $it8 = self.node.links.iterator();
#                while( $it8.hasNext() ) { var n = $it8.next();
#                s += n.id + "<br>";
#                }}
#        }
#        if(Game.debugVis) {
#                s += ("Node " + self.node.id) + "<br>";
#                {
#                        var _g1 = 0, _g = Game.numCults;
#                        while(_g1 < _g) {
#                                var i = _g1++;
#                                s += self.node.visibility[i] + "<br>";
#                        }
#                }
#        }
        width = 0
        if self.node.isGenerator:
            width = 3
        if self.node.owner is not None:
            color = (50, 50, 50)
            if self.node.owner.origin == self.node:
                width = 7
            if self.node.isProtected:
                color = (100,100,100)
            pygame.draw.rect(self.ui.screen, (255,255,255), Rect(x0+self.node.x, y0+self.node.y, options.UI.markerWidth, options.UI.markerHeight), width)
            pygame.draw.rect(self.ui.screen, color, Rect(x0+self.node.x, y0+self.node.y, options.UI.markerWidth, options.UI.markerHeight))
            s = font.render(str(self.node.level + 1), True, options.Game.cultColors[self.node.owner.id])
            self.ui.screen.blit(s, (x0+self.node.x+options.UI.markerWidth*.3, y0+self.node.y+options.UI.markerHeight*.2))
        else:
            pygame.draw.rect(self.ui.screen, (255,255,255), Rect(x0+self.node.x, y0+self.node.y, options.UI.markerWidth, options.UI.markerHeight), width)
            pygame.draw.rect(self.ui.screen, (50, 50, 50), Rect(x0+self.node.x, y0+self.node.y, options.UI.markerWidth, options.UI.markerHeight))
            s = font.render(options.Game.powerShortNames[self.node.displayPower], True, options.Game.powerColors[self.node.displayPower])
            self.ui.screen.blit(s, (x0+self.node.x+options.UI.markerWidth*.3, y0+self.node.y+options.UI.markerHeight*.2))

#        if(self.node.owner != None and self.node.owner.origin == self.node) s += ("<span style='color:" + Game.cultColors[self.node.owner.id]) + "'>The Origin</span><br>";
#        s += self.node.name + "<br>";
#        if(self.node.owner != None) s += ((("<b>" + Game.followerNames[self.node.level]) + "</b> <span style='color:white'>L") + (self.node.level + 1)) + "</span><br>";
#        {
#                s += "<br>";
#                {
#                        var _g1 = 0, _g = Game.numPowers;
#                        while(_g1 < _g) {
#                                var i = _g1++;
#                                if(self.node.power[i] > 0) {
#                                        s += ((((("<b style='color:" + Game.powerColors[i]) + "'>") + Game.powerNames[i]) + "</b> ") + self.node.power[i]) + "<br>";
#                                        self.marker.innerHTML = Game.powerShortNames[i];
#                                        self.marker.style.color = Game.powerColors[i];
#                                }
#                        }
#                }
#                if(self.node.owner == None or self.node.owner.isAI) s += ("Chance of success: <span style='color:white'>" + self.game.player.getGainChance(self.node)) + "%</span><br>";
#        }
#        self.marker.style.background = "#111";
#        if(self.node.owner != None) {
#                self.marker.innerHTML = "" + (self.node.level + 1);
#                self.marker.style.color = "#ffffff";
#                self.marker.style.background = Game.nodeColors[self.node.owner.id];
#        }
#        self.marker.style.border = "1px solid #777";
#        var borderWidth = 1;
#        if(self.node.isGenerator) {
#                borderWidth = 3;
#                var col = "#777";
#                var type = "solid";
#                if(self.node.isProtected) col = "#ffffff";
#                {
#                        var _g = 0, _g1 = self.game.cults;
#                        while(_g < _g1.length) {
#                                var p = _g1[_g];
#                                ++_g;
#                                if(p.origin == self.node and !p.isDead) {
#                                        borderWidth = 5;
#                                        type = "double";
#                                        break;
#                                }
#                        }
#                }
#                self.marker.style.border = (((borderWidth + "px ") + type) + " ") + col;
#                s += "<br>Generates:<br>";
#                {
#                        var _g1 = 0, _g = Game.numPowers;
#                        while(_g1 < _g) {
#                                var i = _g1++;
#                                if(self.node.powerGenerated[i] > 0) s += ((((("<b style='color:" + Game.powerColors[i]) + "'>") + Game.powerNames[i]) + "</b> ") + self.node.powerGenerated[i]) + "<br>";
#                        }
#                }
#        }
#        self.marker.title = s;
#        new JQuery("#map\\.node" + self.node.id).tooltip({ delay : 0});
#        self.markerHL.style.visibility = ((self.node.isHighlighted?"visible":"hidden"));
#        if(self.node.isHighlighted) {
#                self.markerHL.style.width = (UI.markerWidth + borderWidth) + 19;
#                self.markerHL.style.height = (UI.markerHeight + borderWidth) + 19;
#                self.markerHL.style.left = self.node.centerX - ((UI.markerWidth - borderWidth) + 22) / 2;
#                self.markerHL.style.top = self.node.centerY - ((UI.markerHeight - borderWidth) + 22) / 2;
#        }
#}


# INVESTIGATOR

class Investigator:
    def __init__(self, c, ui):
        self.cult = c;
        self.ui = ui;
        self.level = 0;
        self.will = 1;
        self.numTurn = 0;
        self.isHidden = True;

    def gainWill(self):
        if(random.randint(0, 100) > 70 * options.Static.difficulty['investigatorGainWill']):
            return;
        oldLevel = self.level;
        self.will += 1;
        self.level = int(self.will / 3);
        if(self.level > 2):
            self.level = 2;
        if(self.level > oldLevel):
            self.ui.log(((("The investigator of " + self.cult.getFullName()) + " has gained level ") + str(self.level + 1)) + ".");

    def getKillChance(self):
        if(self.cult.awareness <= 5):
            return int(20 * options.Static.difficulty['investigatorKill']);
        elif(self.cult.awareness <= 10):
            return int(65 * options.Static.difficulty['investigatorKill']);
        else:
            return int(70 * options.Static.difficulty['investigatorKill']);

    def killFollower(self):
        if(random.randint(0, 100) > self.getKillChance()):
            return;
        node = None;
        if(self.cult.isRitual):
            for n in self.cult.nodes.q:
                if(n.level > self.level or n.isProtected):
                    continue;
                if(node is not None and n.level <= node.level):
                    continue;
                node = n;
        else:
            for n in self.cult.nodes.q:
                if(n.level > self.level or n.isProtected):
                    continue;
                node = n;
                if(random.random() > 0.5):
                    break;
        if(node is None):
            return;
        self.ui.log(("The investigator revealed the " + self.cult.getFullName()) + " follower.");
        node.removeOwner();
        if(node.visibility[self.cult.game.currentPlayer]):
            node.setHighlighted(True);

    def turn(self):
        if(self.isHidden and self.numTurn > options.Static.difficulty['investigatorTurnVisible']):
            self.ui.log(self.cult.getFullName() + " has found out the investigator's location.");
            self.isHidden = False;
        if(self.will >= 9):
            self.isHidden = True;
        self.numTurn+=1;
        for i in range(self.level):
            self.killFollower();
        if(self.cult.awareness < 5 and not self.cult.isRitual):
            return;
        self.gainWill();
        if(self.cult.isRitual and random.randint(0, 100) < 30):
            self.gainWill();



#

def checkInput():
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
        elif event.type == KEYDOWN:
            if event.key == K_ESCAPE:
                pygame.quit()
            elif event.key == ord("n"):
                return "nextTurn"
            elif event.key == ord("s"):
                return "step"


class MyAI(CultAI):
    def __init__(self, game, ui, id, infoId):
        CultAI.__init__(self, game, ui, id, infoId)
        self.name = "MyAI"





class Challenger:
    def __init__(self,cult):
        self.cult = cult
        self.fought = 0
        self.won = 0

class GameMaster:
    def __init__(self,challengers):
        self.players = challengers
    def Accumulate(self,indexes):
        acc = 0
        for idx in indexes:
            acc += self.players[idx].fought
        return acc
    def SelectIndexes(self,numFighters):
        random.shuffle(self.players)
        indexes = self.Exclude(range(len(self.players)), numFighters)
        return indexes
    def PickChallengers(self,indexes):
        ret = []
        for idx in indexes:
            ret.append(self.players[idx].cult)
            self.players[idx].fought += 1
        return ret
    def Exclude(self,indexes, num):
        while len(indexes) > num:
            acc = self.Accumulate(indexes)
            kill = random.randint(0, acc)
            for i in range(len(indexes)):
                kill -= self.players[indexes[i]].fought
                if kill <= 0:
                    killIdx = i
                    break
            indexes.pop(killIdx)
        return indexes
    def __str__(self):
        s = "GameMaster:"
        for chal in self.players:
            s += "\n  "+str(chal.fought)+"\t"+str(chal.cult)+"\t"+str(chal.won)
            if chal.fought > 0:
                s += "\t("+str((100.0*chal.won)/chal.fought)+")"
        return s
    def Print(self,challengers):
        print(self.players)
        print "Challengers:"
        for i in range(len(challengers)):
            print "  "+str(i)+": "+str(challengers[i])
    def OneRound(self, numPlayers=4, round=0):
        game = Game()

        indexes = self.SelectIndexes(numPlayers)
        challengers = self.PickChallengers(indexes)
        game.restart(challengers)
        while not game.isFinished:
            game.endTurn()
        pygame.display.flip()
        self.players[indexes[game.currentPlayer]].won += 1

        print "Round "+str(round)
        print self
        
gm = GameMaster([Challenger(CultAI),
                 Challenger(CultAI),
                 Challenger(CultAI),
                 Challenger(CultAI),
                 Challenger(CultAI),
                 Challenger(MyAI)])

#random.seed(0)
pygame.init()
font = pygame.font.Font(None, 16)
options = Options()
# about 720 matches per hour
for i in range(100):
    gm.OneRound(4,i)
