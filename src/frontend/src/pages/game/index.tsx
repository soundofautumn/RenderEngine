import * as React from 'react';

import { backend_endpoint, engine_name, game_engine_fps as engine_fps, client, retry_max_times } from '../../utils/client';
import { drawFunctions } from '../../utils/drawFuncs';

import './index.css'

type ControlKey = 'ArrowUp' | 'ArrowDown' | 'ArrowLeft' | 'ArrowRight' | 'Space';
type Direction = 'up' | 'down' | 'left' | 'right' | undefined;

interface IChess {
  type: 'empty' | 'wire' | 'router';
  owner?: number;
  primitive?: number[];
}

const OwnerColors = [
  '#2ecc71',
  '#e67e22',
  '#e74c3c',
  '#3498db',
  '#f1c40f',
  '#9b59b6',
];

function DeletePrimitive(index: number) {
  return client("/engine/primitive/modify", {
    data: {
      Primitive: {
        Line: {
          p1: {
            x: 0,
            y: 0,
          },
          p2: {
            x: 0,
            y: 0,
          },
          algorithm: 0,
        }
      },
      Index: index,
    }
  })
}

function ChangeColor(color: string): Promise<number> {
  return new Promise<number>((resolve, reject) => {
    client('/engine/primitive/push_back', {
      data: {
        PenOptions: {
          color: {
            r: parseInt(color.slice(1, 3), 16),
            g: parseInt(color.slice(3, 5), 16),
            b: parseInt(color.slice(5, 7), 16),
            a: 255,
          },
          width: 1,
          type: 0,
          dash: 1,
          fill_color: { r: 0, g: 0, b: 0, a: 0 },
        },
      }
    }).then(r => {
      resolve(parseInt(r.data.message))
    }).catch(reject);
  })
}

async function DrawSelectBox(x: number, y: number, owner: number): Promise<number[]> {
  const color = OwnerColors[owner] || '#ffffff';
  const primitives: number[] = [
    await ChangeColor(color),
    await drawFunctions.Rectangle.draw({
      pointers: [
        { x, y },
        { x: x + 100, y: y + 100 }
      ]
    }),
  ];
  return primitives;
}

async function DrawRouter(x: number, y: number, owner: number): Promise<number[]> {
  const color = OwnerColors[owner] || '#ffffff';
  const primitives: number[] = [
    await ChangeColor(color),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 10 }
      ]
    }),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 20 }
      ]
    }),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 25 }
      ]
    })
  ];
  console.log(`(${x}, ${y}) ${owner}`, 'Router Drawn:', primitives);
  return primitives;
}

async function PutWire(from: { x: number, y: number }, to: { x: number, y: number }, owner: number) {
  const color = OwnerColors[owner] || '#ffffff';
  const primitives: number[] = [
    await ChangeColor(color),
    await drawFunctions.Line.draw({
      pointers: [
        { x: from.x * 100 + 50, y: from.y * 100 + 50 },
        { x: to.x * 100 + 50, y: to.y * 100 + 50 }
      ]
    }),
  ];
  console.log(`(${from.x}, ${from.y}) -> (${to.x}, ${to.y}) ${owner}`, 'Wire Drawn:', primitives);
  return primitives;
}

function GenerateChessboard(): IChess[][] {
  const emptyBoard: IChess[][] = Array.from({ length: 6 }, () => Array.from({ length: 8 }, () => ({ type: 'empty' })));
  emptyBoard[0][3] = { type: 'router', owner: 0 };
  emptyBoard[0][4] = { type: 'router', owner: 3 };
  emptyBoard[0][7] = { type: 'router', owner: 1 };
  emptyBoard[1][2] = { type: 'router', owner: 2 };
  emptyBoard[1][3] = { type: 'router', owner: 3 };
  emptyBoard[2][2] = { type: 'router', owner: 1 };
  emptyBoard[3][0] = { type: 'router', owner: 0 };
  emptyBoard[3][5] = { type: 'router', owner: 2 };
  return emptyBoard;
}

export default function Game() {
  const canvasRef = React.useRef<HTMLCanvasElement>(null);

  /* 处理键盘事件 */
  const [pressedKeys, setPressedKeys] = React.useState<ControlKey[]>([]);
  React.useEffect(() => {
    document.getElementById('pressed-key-list')!.scrollTo({
      left: document.getElementById('pressed-key-list')!.scrollWidth,
      behavior: 'smooth',
    });
  }, [pressedKeys])
  const handleKeyPressed = (e: React.KeyboardEvent<HTMLElement>) => {
    const extendKeyMap: { [key: string]: ControlKey } = {
      w: 'ArrowUp',
      a: 'ArrowLeft',
      s: 'ArrowDown',
      d: 'ArrowRight',
      ' ': 'Space',
    }
    if (e.key === 'ArrowUp' || e.key === 'ArrowDown' || e.key === 'ArrowLeft' || e.key === 'ArrowRight') {
      e.preventDefault();
      handleGameKey(e.key as ControlKey);
    }
    if (Object.keys(extendKeyMap).includes(e.key)) {
      e.preventDefault();
      handleGameKey(extendKeyMap[e.key]);
    }
  }
  const handleGameKey = (key: ControlKey) => {
    const directionMap: { [key in ControlKey]: Direction } = {
      'ArrowUp': 'up',
      'ArrowDown': 'down',
      'ArrowLeft': 'left',
      'ArrowRight': 'right',
      'Space': undefined,
    };
    setPressedKeys([...pressedKeys, key]);
    if (key === 'Space') handleSelect();
    else handleMove(directionMap[key]);
  }

  /* 连接后端 */
  const [, setLoading] = React.useState(true);
  const loadingRef = React.useRef(true);
  const fpsRef = React.useRef(0);
  const [fps, setFps] = React.useState(0);
  const first = React.useRef(true);
  const [start, setStart] = React.useState(true);
  const startTimeoutRef = React.useRef<number | null>(null);
  const [errorMessage, setErrorMessage] = React.useState('等待后端响应...');
  React.useEffect(() => {
    if (!first.current) return;
    first.current = false;

    const width = 800;
    const height = 600;

    startTimeoutRef.current = setTimeout(() => {
      setStart(false);
    }, 100);

    const ConnectToBackend = (retryTimes = 0) => new Promise((resolve, reject) => {
      client('/engine/create', {
        data: {
          name: engine_name,
          width,
          height,
        },
        timeout: 1000,
      }).then(r => {
        console.log(r.data);
        console.log(`Engine ${engine_name} created with ${width}x${height}`);
        setInterval(() => {
          setFps(fpsRef.current);
          if (fpsRef.current === 0) {
            window.location.reload();
          }
        }, 500);
        Init();
        resolve(1);
      }).catch(e => {
        console.error(e);
        setErrorMessage(`后端无响应，重新尝试连接.. ${retryTimes + 1}/${retry_max_times}`);
        if (retryTimes >= retry_max_times) {
          reject('Failed to create engine');
          return;
        } else {
          setTimeout(() => {
            ConnectToBackend(retryTimes + 1).then(resolve).catch(reject);
          }, 1000)
        }
      });
    });

    ConnectToBackend().then(() => {
      const ws = new WebSocket(`//${backend_endpoint}/engine/${engine_name}`);
      ws.onopen = function () {
        ws.send(`set_fps ${engine_fps}`);
        setLoading(false);
        if (startTimeoutRef.current) clearTimeout(startTimeoutRef.current);
        setStart(true);
        loadingRef.current = false;
      };
      ws.binaryType = "arraybuffer";
      ws.onmessage = function (event) {
        if (!canvasRef.current) return;

        fpsRef.current += 1;
        setTimeout(() => {
          fpsRef.current -= 1;
        }, 1000)

        const ctx = canvasRef.current.getContext("2d");
        canvasRef.current.width = width;
        canvasRef.current.height = height;
        const data = new Uint8ClampedArray(event.data);

        const imageData = new ImageData(data, width, height);
        if (ctx)
          ctx.putImageData(imageData, 0, 0);
      };
    }).catch(e => {
      console.error(e);
      setErrorMessage("后端无响应，请检查后端是否正常运行。");
    });
  }, [])

  /* 游戏逻辑 */
  const [chessboard, setChessboard] = React.useState<IChess[][]>([]);
  const [currentPosition, setCurrentPosition] = React.useState({ x: -1, y: -1 });
  const currentWiresPrimitive = React.useRef<number[]>([]);
  const allWiresPrimtives = React.useRef<{ [owner: number]: number[] }>({});
  const handleMove = (direction: Direction) => {
    const putWire = async (x: number, y: number) => {
      if (currentOwner < 0) {
        setCurrentPosition({ x, y });
        return;
      } else {
        if (chessboard[y][x].type === 'empty') {
          // 空的地方放线
          chessboard[y][x] = { type: 'wire', owner: currentOwner };
          currentWiresPrimitive.current.push(...await PutWire(currentPosition, { x, y }, currentOwner));
          setCurrentPosition({ x, y });
        }
        else if (chessboard[y][x].type === 'wire') {
          // 线不能交叉
          return;
        } else if (chessboard[y][x].type === 'router') {
          // 不是自己的路由器不能进
          if (chessboard[y][x].owner !== currentOwner || currentStartChess && y === currentStartChess.y && x === currentStartChess.x)
            return;
          else {
            // 成功连线
            currentWiresPrimitive.current.push(...await PutWire(currentPosition, { x, y }, currentOwner));
            allWiresPrimtives.current[currentOwner] = currentWiresPrimitive.current;
            currentWiresPrimitive.current = [];
            setCurrentPosition({ x, y });
            setCurrentOwner(-1);
            setCurrentStartChess(null);
            checkWin();
          }
        }
      }
    }
    if (direction === 'up') {
      if (currentPosition.y > 0) putWire(currentPosition.x, currentPosition.y - 1);
    }
    if (direction === 'down') {
      if (currentPosition.y < 5) putWire(currentPosition.x, currentPosition.y + 1);
    }
    if (direction === 'left') {
      if (currentPosition.x > 0) putWire(currentPosition.x - 1, currentPosition.y);
    }
    if (direction === 'right') {
      if (currentPosition.x < 7) putWire(currentPosition.x + 1, currentPosition.y);
    }
  }
  const [currentOwner, setCurrentOwner] = React.useState(-1);

  const curPosPrimitives = React.useRef<number[]>([]);
  React.useEffect(() => {
    console.log('position', currentPosition, curPosPrimitives.current);
    curPosPrimitives.current.forEach(primitive => {
      DeletePrimitive(primitive);
    })
    DrawSelectBox(currentPosition.x * 100, currentPosition.y * 100, currentOwner).then(r => {
      curPosPrimitives.current = r;
    })
  }, [currentPosition, currentOwner])
  const [currentStartChess, setCurrentStartChess] = React.useState<{ x: number, y: number } | null>(null);
  const handleSelect = () => {
    const currentChess = chessboard[currentPosition.y][currentPosition.x];
    console.log('handleSelect', currentChess);
    if (currentChess.type === 'router') {
      const owner = currentChess.owner === undefined ? -1 : currentChess.owner;
      if (owner === currentOwner) {
        // 取消连线
        setCurrentOwner(-1);
      }
      else {
        // 开始连线
        if (allWiresPrimtives.current[owner]) {
          allWiresPrimtives.current[owner].forEach(primitive => {
            DeletePrimitive(primitive);
          })
          allWiresPrimtives.current[owner] = [];
        }
        setCurrentOwner(owner);
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === owner ? { type: 'empty' } : chess)));
        setCurrentStartChess({ x: currentPosition.x, y: currentPosition.y });
      }
    } else if (currentChess.type === 'wire') {
      if (currentOwner === -1) {
        // 删除已经连过的线
        const owner = currentChess.owner === undefined ? -1 : currentChess.owner;
        allWiresPrimtives.current[owner].forEach(primitive => {
          DeletePrimitive(primitive);
        })
        allWiresPrimtives.current[owner] = [];
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === owner ? { type: 'empty' } : chess)));
      } else {
        // 取消当前连线
        currentWiresPrimitive.current.forEach(primitive => {
          DeletePrimitive(primitive);
        })
        currentWiresPrimitive.current = [];
        setCurrentOwner(-1);
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === currentOwner ? { type: 'empty' } : chess)));
        setCurrentStartChess(null);
      }
    }
  }
  const checkWin = () => {
    const owners = new Set<number>();
    chessboard.forEach(row => row.forEach(chess => {
      if (chess.type === 'router') owners.add(chess.owner || -1);
    }));
    const connectedOwners = new Set<number>();
    chessboard.forEach(row => row.forEach(chess => {
      if (chess.type === 'wire') connectedOwners.add(chess.owner || -1);
    }));
    if (owners.size === connectedOwners.size) {
      alert('You Win!');
    }
  }
  const Init = async () => {
    await ChangeColor('#ffffff');
    const newChessboard = GenerateChessboard();
    setChessboard(newChessboard)
    for (const row of newChessboard) {
      for (const chess of row) {
        if (chess.type === 'router') {
          await DrawRouter(row.indexOf(chess) * 100 + 50, newChessboard.indexOf(row) * 100 + 50, chess.owner === undefined ? -1 : chess.owner);
        }
      }
    }
    setCurrentPosition({ x: 0, y: 0 });
  }


  return <div
    id="game"
    onKeyDown={handleKeyPressed}
    tabIndex={0}
  >
    <div id="hover" style={{ opacity: start ? 0 : 1 }}>
      <div>{errorMessage}</div>
    </div>
    <div id="mousePosition">
      Engine: {engine_name};
      FPS: {fps}.</div>
    <p id='title'>
      <span style={{ color: "#2ecc71" }}>断</span>
      <span style={{ color: "#f1c40f" }}>线</span>
      <span style={{ color: "#74b9ff" }}>重</span>
      <span style={{ color: "#e74c3c" }}>连</span>
    </p>
    <div id='hide'>
      {
        chessboard.map((row, y) => row.map((chess, x) => {
          return <div key={`${x}-${y}`} className={`chess ${chess.type} ${chess.owner === undefined ? -2 : chess.owner}`} style={{ top: y * 100, left: x * 100 }} />
        }))
      }
      <div className={`chess current ${currentOwner}`} style={{ top: currentPosition.y * 100, left: currentPosition.x * 100 }} />
    </div>
    <canvas
      width={800}
      height={600}
      ref={canvasRef}
    />
    <div id='pressed-key-list'>
      {
        pressedKeys.map((key, index) => <div key={index} className={`key-icon ${key}`} />)
      }
    </div>
  </div>
}